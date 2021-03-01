#include <errno.h>
#include <fcntl.h>
#include <linux/types.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

static int32_t pm_qos_fd = -1;

int32_t low_latency(int32_t target)
{
	uint32_t sot = sizeof(target);
	uint32_t cwbytes = 0;

	if (sot <= 1) {
		fprintf(stderr,
			"Error: Invalid target size: %d bytes.\n",
				sot);
		return 1;
	}
	if (pm_qos_fd >= 0) {
		fprintf(stderr,
			"Error: PM QoS fd: %d.\n",
				pm_qos_fd);
		return 1;
	}
	pm_qos_fd = open("/dev/cpu_dma_latency",
		O_RDWR);
	if (pm_qos_fd < 0) {
		fprintf(stderr,
			"Error: Failed to open PM QoS device: %s\n",
				strerror(errno));
		return 1;
	}
	cwbytes = write(pm_qos_fd,
		&target,
			sot);
	if (cwbytes != sot) {
		fprintf(stderr,
			"Error: Wrote %d (out of %u) bytes requested.\n",
				cwbytes,
					sot);
		return 1;
	}
	if (target > 0) {
		fprintf(stdout,
			"CPU DMA latency constraint set to %d μs\n",
				target);
		return 0;
	} else {
		fprintf(stdout,
			"CPU DMA latency constraint set to immediate (0 μs)\n");
		return 0;
	}
	return 2;
}

int main(int argc, char *argv[])
{
	int32_t llr = 1;

	if (!(argc >= 2)) {
		fprintf(stdout,
			"%s: Set PM QoS CPU DMA latency constraint\n \
				\rUsage: %s <latency constraint in μs> (0...%u)\n",
					argv[0],
						argv[0],
							INT32_MAX);
		return 1;
	}
	int64_t target = atoi(argv[1]);

	if ((int32_t)target < 0 || \
			target > INT32_MAX) {
		fprintf(stderr,
			"Error: Requested value out of range (0...%u)\n",
				INT32_MAX);
		return 1;
	}
	llr = low_latency((int32_t)target);
	if (llr != 0)
		exit(llr);
	pause();
	fprintf(stderr,
		"Warning: pause() failed, falling back to sleep() loop.\n");
	while (!sleep(999))
		;
	fprintf(stderr,
		"Error: sleep() loop failed, aborting.\n");
	exit(1);
	return 2;
}

