# CS165Lab1

Only capable of modifying the first character as a means of skipping sets of strings by changing char0 in the findHash function.

Settings that need to change for different passwords and hashes: PASSWORDSIZE, SALT, MAGIC, GOALHASH, SALTSIZE, MAGICSIZE.

Thread counts are most efficient for factors of 26, other factors will cause them to solve duplicate hashes.
