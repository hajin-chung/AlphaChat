#include "const.h"
#include "res.h"
#include "print.h"
#include "utils.h"

void handle_res()
{
	char buf[MAX_REQ_BUF_SIZE];
	int str_len;
	int cmd_code=-1, status_code=-1;

	memset(buf, 0, MAX_REQ_BUF_SIZE);
	str_len = read(req_sock, buf, MAX_REQ_BUF_SIZE);
	if(str_len <= 0)
	{
		print_error("[!] Sever closed");
	}

	buftodata(2, buf, 
		INT, &cmd_code, 0, 4,
		INT, &status_code, 4, 4);

	if(DEBUG_FLAG)
	{
		print_info("<~");
		write(fileno(stdout), buf, MAX_REQ_BUF_SIZE);
		print_info("\n");
	}

	if(status_code == CMD_STATUS_SUCCESS && cmd_code != CMD_SEND_CHAT_CODE)
	{
		printfBox(lobby, "[*] Command %s success", cmd_table[cmd_code]);
	}

	if(status_code == CMD_STATUS_FAIL)
	{
		printfBox(lobby, "[*] Command code %d failed!", cmd_code);		
	}
	else if(cmd_code == CMD_ROOM_LIST_CODE)
	{
		int room_cnt, room_id, offset, i;
		char room_name[ROOM_NAME_MAX_LEN];

		buftodata(1, buf, INT, &room_cnt, 8, 4);

		offset = 12;
		printfBox(lobby, " [ROOM LIST %d]", room_cnt);
		for(i=0 ; i<room_cnt ; i++)
		{
			memset(room_name, 0, ROOM_NAME_MAX_LEN);	
			buftodata(2, buf, 
				INT, &room_id, offset, 4,
				CHAR, room_name, offset+4, ROOM_NAME_MAX_LEN);
			offset += (4 + ROOM_NAME_MAX_LEN);

			memcpy(room_list[room_id], room_name, ROOM_NAME_MAX_LEN);

			printfBox(lobby, "   %s [%d]", room_name, room_id);
		}
	}
	else if(cmd_code == CMD_USER_LIST_CODE)
	{
		int user_cnt, offset, user_status, user_id, i;
		char user_name[USER_NAME_MAX_LEN];

		buftodata(1, buf, INT, &user_cnt, 8, 4);

		offset = 12;
		printfBox(lobby, " [USER LIST %d]", user_cnt);
		for(i=0 ; i<user_cnt ; i++)
		{
			memset(user_name, 0, USER_NAME_MAX_LEN);	
			buftodata(3, buf, 
				INT, &user_id, offset, 4,
				CHAR, user_name, offset+4, USER_NAME_MAX_LEN,
				INT, &user_status, offset+4+USER_NAME_MAX_LEN, 4);
			offset += (8 + USER_NAME_MAX_LEN);

			memcpy(user_list[user_id], user_name, USER_NAME_MAX_LEN);

			printfBox(lobby, "    %s [%d]", user_name, user_id);
		}
	}
	else if(cmd_code == CMD_ROOM_CONNECT_CODE)
	{
	}
	else if(cmd_code == CMD_SEND_CHAT_CODE)
	{
		int user_id, room_id, chat_type;
		char content[CONTENTS_MAX_LEN];

		memset(content, 0, CONTENTS_MAX_LEN);
		buftodata(4, buf, 
			INT, &chat_type, 12, 4,
			INT, &room_id, 16, 4,
			INT, &user_id, 20, 4,
			CHAR, content, 24, CONTENTS_MAX_LEN);

		if(room_id == cur_room_id)
			printfBox(chat, "%s [%d]> %s", user_list[user_id], user_id, content);
	}
	else if(cmd_code == CMD_REGISTER_CODE)
	{
		buftodata(1, buf, INT, &my_user_id, 8, 4);	
		printfBox(lobby, "[*] user id : %d", my_user_id);
	}
	move_cursor_cmd();
}