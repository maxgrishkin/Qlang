#pragma once
#include <fstream>
#include <string>
#include "Parser.h"

inline int TestOnlyExpression(const std::string& code) {
	system("touch test.asm");
	std::fstream file("test.asm");
	if (!file.is_open()) {
		throw std::runtime_error("can't get the test to work");
	}

	file << "global _start\nsection .text\n_start:\n" + code;
	file << "mov rdi, rax\nmov rax, 60\nsyscall\n";
	file.close();

	system("nasm -f elf64 test.asm -o test.o");
	system("ld -o test test.o");

	const int status = system("./test");

	int exit_code = -1;
	if (status != -1) {
		if (WIFEXITED(status)) {
			exit_code = WEXITSTATUS(status);
		}
	}

	system("rm -f test.asm test.o test");
	return exit_code;
}

inline int TestExpressionWithData(const std::string& code, const std::string& data) {
	system("touch test.asm");
	std::fstream file("test.asm");
	if (!file.is_open()) {
		throw std::runtime_error("can't get the test to work");
	}

	file << "global _start\n";
	file << "section .bss\n" + data;
	file << "section .text\n_start:\n" + code;
	file << "mov rdi, rax\nmov rax, 60\nsyscall\n";
	file.close();

	system("nasm -f elf64 test.asm -o test.o");
	system("ld -o test test.o");

	const int status = system("./test");

	int exit_code = -1;
	if (status != -1) {
		if (WIFEXITED(status)) {
			exit_code = WEXITSTATUS(status);
		}
	}

	system("rm -f test.asm test.o test");
	return exit_code;
}

inline int TestFullCode(const qlang::SectionsCode& sections_code, qlang::highCompiler& hc, const std::vector<const qlang::Type*>& argMain) {
	system("touch test.asm");
	std::fstream file("test.asm");
	if (!file.is_open()) {
		throw std::runtime_error("can't get the test to work");
	}

	file << "global _start\n";
	file << "section .bss\n" + sections_code.data;
	file << "section .text\n_start:\ncall " + qlang::Parser::FuncToManglingName(hc.getFunctions().at("main")[argMain]) + "\nmov rdi, rax\nmov rax, 60\nsyscall\n";
	file << sections_code.otherFunks;
	file.close();

	system("nasm -f elf64 test.asm -o test.o");
	system("ld -o test test.o");

	const int status = system("./test");

	int exit_code = -1;
	if (status != -1) {
		if (WIFEXITED(status)) {
			exit_code = WEXITSTATUS(status);
		}
	}

	system("rm -f test.asm test.o test");
	return exit_code;
}

inline int TestObjectFiles(const std::vector<std::string>& objectFiles) {
	std::string command = "ld -o test";
	for (const auto& object : objectFiles) {
		command += ' ' + object;
	}
	system(command.c_str());

	command = "rm -f";
	for (const auto& object : objectFiles) {
		command += ' ' + object;
	}
	system(command.c_str());

	const int status = system("./test");

	system("rm -f test");

	int exit_code = -1;
	if (status != -1) {
		if (WIFEXITED(status)) {
			exit_code = WEXITSTATUS(status);
		}
	}

	return exit_code;
}