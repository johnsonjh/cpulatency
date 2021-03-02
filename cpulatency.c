#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static int32_t pm_qos_fd = -1;

int32_t low_latency(
		int32_t target
) {

	uint32_t sot = sizeof(
		target
	);
	uint32_t cwbytes = 0;

	if (
		sot <= 1
	) {
		fprintf(
			stderr,
			"Error: Invalid target size: %u bytes.\n",
			sot
		);
		return 1;
	}

	if (
		pm_qos_fd >= 0
	) {
		fprintf(
			stderr,
			"Error: PM QoS fd: %d.\n",
			pm_qos_fd
		);
		return 1;
	}

	pm_qos_fd = open(
		"/dev/cpu_dma_latency",
		O_RDWR
	);

	if (
		pm_qos_fd < 0
	) {
		fprintf(
			stderr,
			"Error: Failed to open PM QoS device: %s.\n",
			strerror(
				errno
			)
		);
		return 1;
	}

	cwbytes = write(
		pm_qos_fd,
		&target,
		sot
	);

	if (
		cwbytes != sot
	) {
		fprintf(
			stderr,
			"Error: Only wrote %u out of %u bytes requested.\n",
			cwbytes,
			sot
		);
		return 1;
	}

	fprintf(
		stdout,
		"CPU DMA latency limit set to %d μs\n",
		target
	);
	return 0;
}

int main(
		int argc,
		char *argv[]
) {

	int32_t llr = 1;

	if (
		!(
			argc >= 2
		)
	) {
		fprintf(
			stdout,
			"%s: Set PM QoS CPU DMA latency limit\n \
			\rUsage: %s <latency limit in μs> (0 ⟷ %d)\n",
			argv[0],
			argv[0],
			INT32_MAX
		);
		return 1;
	}

	uint32_t ditr;

	size_t dsiz = snprintf(
		NULL,
		0,
		"%d",
		INT32_MAX
	);

	char *dstr = malloc(
		dsiz + 1
	);

	if (
		dstr == NULL || \
		!dstr
	) {
			fprintf(
				stderr,
				"Error: malloc() failure.\n"
			);
		exit(
			1
		);
	}

	size_t dmax = snprintf(
		dstr,
		dsiz + 1,
		"%d",
		INT32_MAX
	);

	free(
		dstr
	);

	size_t dlen = strnlen(
		argv[1],
		dmax
	);
	size_t dact = strlen(
		argv[1]
	);

	if (
		dact > dmax
	)
		argv[1][0] = 0;
	for (
		ditr = 0;
		ditr < (uint32_t)dlen;
		++ditr
	) {
		if (
			argv[1][ditr] < 48 || \
			argv[1][ditr] > 57
		) {
			fprintf(
				stderr,
				"Error: Value must be a non-negative integer.\n"
			);
			return 1;
		}
	}

	int64_t target = atoi(
		argv[1]
	);

	if (
		(int32_t)target < 0 || \
		target > INT32_MAX
	) {
		fprintf(
			stderr,
			"Error: Value out of range (0 ⟷ %d)\n",
			INT32_MAX
		);
		return 1;
	}

	llr = low_latency(
		(int32_t)target
	);

	if (
		llr != 0
	)
		exit(
			llr
		);

	pause();

	fprintf(
		stderr,
		"Warning: pause() failed, falling back to sleep() loop.\n"
	);

	while (
		!sleep(
			999
		)
	)
		;

	fprintf(
		stderr,
		"Error: sleep() loop failed, aborting.\n"
	);

	exit(
		1
	);

	return 2;
}
