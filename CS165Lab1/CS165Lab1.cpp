// CS165Lab1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define OPENSSL_NO_DEPRECATED
#define OPENSSL_API_COMPAT 0x10101000L

#include <iostream>
#include <string>
#include <bitset>
#include <openssl/md5.h>
#include <chrono>
#include <thread>
#include <vector>
#include <include\openssl\md5.h>


using namespace std;

const string SALT = "4fTgjp6q";		//team 27 "hfT7jp2q";
const string MAGIC = "$1$";
const string GOALHASH = "0s6haNhWTl/ejCBFXdJRj1"; /*"wPwz7GC6xLt9eQZ9eJkaq.";*/
const string ascii64 = "./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
const string alphabet = "abcdefghijklmnopqrstuvwxyz";
const int PASSWORDSIZE = 6;
const int SALTSIZE = 8;
const int MAGICSIZE = 3;
const int MD5SIZE = 16;


string generateHash(string password, string salt, string magic);
string findHash(int passwordLen, int incrementn, int incrementm, int char5, int char4, int char3, int char2, int char1, int char0);

int main()
{
	
	string password = "aaaaaa";
	bool found = false;
	string hash;
	long hashes = 0;
	auto start_time = std::chrono::high_resolution_clock::now();
	auto end_time = std::chrono::high_resolution_clock::now();
	auto time = end_time - start_time;
	
	
	const int numThreads = 13;
	
	vector<std::thread> threads;
	
	for (int i = 0; i < numThreads; i++) {
		threads.push_back(std::thread(findHash, PASSWORDSIZE, numThreads, 1, i, 0, 0, 0, 0, 7));
	}

	for (std::thread& t : threads) {
		if (t.joinable())
			t.join();
	}

	
	password = "zahroy";
	hash = generateHash(password, SALT, MAGIC);
	if (hash == "NFqHx2WW/kpjgFaQz0C810")
		std::cout << "matches" << endl;
	std::cout << "hash: " << hash << "\ndesired: NFqHx2WW/kpjgFaQz0C810" << endl;
	

}

string findHash(int passwordLen, int incrementn, int incrementm, int char5, int char4, int char3, int char2, int char1, int char0) {
	string password;
	password.resize(passwordLen);
	string hash;
	int hashes = 0;
	bool found = false;
	auto start_time = std::chrono::high_resolution_clock::now();
	auto end_time = std::chrono::high_resolution_clock::now();
	auto time = end_time - start_time;

	for (short i = char0; i < alphabet.size(); i++) {
		password[0] = alphabet[i];
		for (short j = char1; j < alphabet.size(); j++) {
			password[1] = alphabet[j];
			for (short k = char2; k < alphabet.size(); k++) {
				password[2] = alphabet[k];
				for (short l = char3; l < alphabet.size(); l++) {
					password[3] = alphabet[l];
					for (short m = char4; m < alphabet.size(); m += incrementm) {
						password[4] = alphabet[m];
						for (short n = char5; n < alphabet.size(); n += incrementn) {
							password[5] = alphabet[n];
							hash = generateHash(password, SALT, MAGIC);
							hashes++;
							if (hash == GOALHASH) {
								found = true;
								break;
							}
						}
						if (found)break;

					}
					if (hashes > 2999) {
						end_time = std::chrono::high_resolution_clock::now();
						time = end_time - start_time;
						std::cout << "time:" << time / std::chrono::milliseconds(1) << " hashes per second: " << (hashes / ((time / std::chrono::milliseconds(1)) / 1000.0)) << endl;
						std::cout << "current password: " << password << endl;
						hashes = 0;
						start_time = std::chrono::high_resolution_clock::now();
					}
					if (found)break;

				}
				if (found)break;

			}
			if (found)break;

		}
		if (found)break;

	}

	while(found)
		std::cout << "I FOUND IT! " << found << " password is: " << password << endl;
	
	return password;
}


string generateHash(string password, string salt, string magic) {
	string alternateString = password + salt + password;
	unsigned char* alternate = (unsigned char*)alternateString.c_str();
	unsigned char alternateHash[MD5SIZE];
	MD5(alternate, 2 * PASSWORDSIZE + SALTSIZE, alternateHash);	//generate the alternate
	
	short pwdsize = PASSWORDSIZE;
	unsigned char alternate_substr[PASSWORDSIZE];
	memcpy(alternate_substr, alternateHash, PASSWORDSIZE);
	string intermediate(alternate_substr, alternate_substr + PASSWORDSIZE);
	intermediate = password + magic + salt + intermediate;

	unsigned char intermediateHash[MD5SIZE];
	char passwordFirst = password[0];

	while (pwdsize > 0) {
		if (pwdsize & 1) {
			intermediate.push_back('\0');
		}
		else {
			intermediate = intermediate + passwordFirst;
		}
		pwdsize = pwdsize >> 1;
	}


	unsigned char* intermediate_char = (unsigned char*)intermediate.c_str();
	MD5(intermediate_char, intermediate.size(), intermediateHash);

	string nextIntermediate;
	string temp;
	for (short i = 0; i < 1000; i++) {
		if (i % 2 == 0) nextIntermediate.assign(intermediateHash, intermediateHash + MD5SIZE);
		else nextIntermediate = password;
		
		if (i % 3 != 0) nextIntermediate += salt;
		
		if (i % 7 != 0) nextIntermediate += password;
		
		if (i % 2 == 0) nextIntermediate += password;
		else {
			temp.assign(intermediateHash, intermediateHash + MD5SIZE);
			nextIntermediate += temp;
		}
		
		unsigned char* nextChar = (unsigned char*)nextIntermediate.c_str();
		MD5(nextChar, nextIntermediate.size(), intermediateHash);
	}

	int order[] = { 11, 4, 10, 5, 3, 9, 15, 2, 8, 14, 1, 7, 13, 0, 6, 12 };
	unsigned char rearranged[MD5SIZE];
	for (int i = 0; i < 16; i++) {
		rearranged[i] = intermediateHash[order[i]];
	}

	const size_t N = 16;  // bound on string length
	bitset<N * 8> b;
	for (int i = 0; i < MD5SIZE; i++) {
		char c = rearranged[i];
		for (int j = 7; j >= 0 && c; --j) {
			if (c & 0x1) {
				b.set(8 * i + j);
			}
			c >>= 1;
		}
	}

	string output = "";
	bitset<6> curChar;
	for (int i = 127; i > 0; i -= 6) {
		
		if (i > 1) {
			curChar[5] = b[i - 5];
			curChar[4] = b[i - 4];
			curChar[3] = b[i - 3];
			curChar[2] = b[i -2 ];
			curChar[1] = b[i- 1];
			curChar[0] = b[i];
		}
		else {
			curChar[5] = 0;
			curChar[4] = 0;
			curChar[3] = 0;
			curChar[2] = 0;
			curChar[1] = b[i - 1];
			curChar[0] = b[i];
		}
		
		int charPos = (int)(curChar.to_ulong());
		output += ascii64[charPos];
	}

	return output;
}



// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
