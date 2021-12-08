#pragma once

enum class OperationCode
{
	STOP,
	CHECK_SIZE,
	CHECK_NAME,
	CHECK_LOGIN,
	REGISTRATION,
	SIGN_IN,
	NEW_MESSAGES,
	GET_NUMBER_MESSAGES_IN_CHAT,
	COMMON_CHAT_GET_MESSAGE,
	COMMON_CHAT_ADD_MESSAGE,
	READY,
	ERROR,
};

const int HEADER_SIZE = 256;
