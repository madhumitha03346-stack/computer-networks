int findMaximumXOR(int* nums, int numsSize) {
    int maxXor = 0;

    for (int i = 0; i < numsSize; i++) {
        for (int j = i + 1; j < numsSize; j++) {
            int x = nums[i] ^ nums[j];

            if (x > maxXor) {
                maxXor = x;
            }
        }
    }

    return maxXor;
}