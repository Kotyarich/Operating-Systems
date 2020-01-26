#include <stdio.h>
#include <stdbool.h>
#include <windows.h>

#define READERS 5
#define WRITERS 3
#define WRITERS_ITERS 7
#define SLEEP_TIME 300

HANDLE writers[WRITERS];
HANDLE readers[READERS];

HANDLE mutex;
HANDLE can_read;
HANDLE can_write;
volatile LONG waiting_writers = 0;
volatile LONG waiting_readers = 0;
volatile LONG active_readers = 0;
bool is_writer_active = false;

volatile int value = 0;

void start_read(void) {
	InterlockedIncrement(&waiting_readers);
	if (is_writer_active || WaitForSingleObject(can_write, 0) == WAIT_OBJECT_0) {
		WaitForSingleObject(can_read, INFINITE);
	}
	// Мьютекс тут не нужен, но Рязанова требует тыкнуть для общего развития
	WaitForSingleObject(mutex, INFINITE);

	InterlockedDecrement(&waiting_readers);
	InterlockedIncrement(&active_readers);

	SetEvent(can_read);

	ReleaseMutex(mutex);
}

void stop_read(void) {
	InterlockedDecrement(&active_readers);

	if (waiting_readers == 0) {
		SetEvent(can_write);
	}
}

DWORD WINAPI reader(LPVOID lpParams) {
	while (value < WRITERS * WRITERS_ITERS) {
		start_read();
		printf("---------------------------------Reader #%ld; read value: %d\n", (int) lpParams, value);
		stop_read();
		Sleep(SLEEP_TIME);
	}

	return EXIT_SUCCESS;
}

void start_write(void) {
	InterlockedIncrement(&waiting_writers);
	if (is_writer_active || active_readers > 0) {
		WaitForSingleObject(can_write, INFINITE);
	}

	InterlockedDecrement(&waiting_writers);
	is_writer_active = true;
	ResetEvent(can_write);
}

void stop_write(void) {
	is_writer_active = false;

	if (!waiting_writers) {
		SetEvent(can_read);
	} else {
		SetEvent(can_write);
	}
}

DWORD WINAPI writer(LPVOID lpParams) {
	for (int i = 0; i < WRITERS_ITERS; ++i) {
		start_write();

		++value;
		printf("Writer #%ld wrote value: %ld\n", (int) lpParams, value);

		stop_write();
		Sleep(SLEEP_TIME);
	}

	return EXIT_SUCCESS;
}

int init_handles(void) {
	if ((mutex = CreateMutex(NULL, FALSE, NULL)) == NULL) {
		perror("CreateMutex");
		return EXIT_FAILURE;
	}
	// Тут создаем события с автоматическим и ручным сбросом соответственно
	// Правильней, проще и логичней было сделать наоборот, но Рязанова
	// видимо не понимает, как работают события, и требует делать так
	if ((can_read = CreateEvent(NULL, FALSE, TRUE, NULL)) == NULL) {
		perror("CreateEvent");
		return EXIT_FAILURE;
	}
	if ((can_write = CreateEvent(NULL, TRUE, TRUE, NULL)) == NULL) {
		perror("CreateEvent");
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

int create_threads(HANDLE *threads, int threads_count, DWORD (*on_thread)(LPVOID)) {
	for (int i = 0; i < threads_count; ++i) {
		if ((threads[i] = CreateThread(NULL, 0, on_thread, (LPVOID) i, 0, NULL)) == NULL) {
			perror("CreateThread");
			return EXIT_FAILURE;
		}
	}

	return EXIT_SUCCESS;
}

int main(void) {
	setbuf(stdout, NULL);

	int rc = EXIT_SUCCESS;

	if ((rc = init_handles()) != EXIT_SUCCESS
	 || (rc = create_threads(writers, WRITERS, writer)) != EXIT_SUCCESS
	 || (rc = create_threads(readers, READERS, reader)) != EXIT_SUCCESS) {
		return rc;
	}

	WaitForMultipleObjects(WRITERS, writers, TRUE, INFINITE);
	WaitForMultipleObjects(READERS, readers, TRUE, INFINITE);

	CloseHandle(mutex);
	CloseHandle(can_read);
	CloseHandle(can_write);

	return rc;
}
