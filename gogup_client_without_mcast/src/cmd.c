#include "const.h"
#include "cmd.h"
#include "utils.h"
#include "print.h"

void handle_cmd()
{
	char cmd[CMD_MAX_LEN];
	char buf[MAX_REQ_BUF_SIZE];
	int code, offset;
	int send_flag = 1; // some commands dont send data to server. a flag for that

	memset(buf, 0, MAX_REQ_BUF_SIZE);
	memset(cmd, 0, CMD_MAX_LEN);

	fgets(cmd, CMD_MAX_LEN, stdin);
	remove_newline(cmd, CMD_MAX_LEN);	
	code = cmdtocode(cmd);

	if(code == CMD_HELP)
	{
		send_flag = 0;
		print_help();
	}

	if(code == -1)
	{
		int type = CHAT_TYPE_TEXT;
		code = CMD_SEND_CHAT_CODE;

		if(cmd[0] == '/')
		{
			printfBox(lobby, "[*] %s: Command not found", cmd);
			send_flag = 0;
		}
		else if(cur_room_id == -1)
		{
			printfBox(lobby, "[*] room not connected");
			send_flag = 0;
		}
		else
		{
			char chat_content[CONTENTS_MAX_LEN];
			memset(chat_content, 0, CONTENTS_MAX_LEN);

			buftodata(1, cmd,
				CHAR, chat_content, 0, CONTENTS_MAX_LEN);

			datatobuf(5, buf, 
				INT, &code, 0, 4,
				INT, &type, 4, 4,
				INT, &cur_room_id, 8, 4,
				INT, &my_user_id, 12, 4,
				CHAR, chat_content, 16, CONTENTS_MAX_LEN);
		}
	}
	else
	{
		offset = strlen(cmd_table[code]);

		if(code == CMD_ROOM_CREATE_CODE) // /create [room_name]
		{
			char room_name[ROOM_NAME_MAX_LEN];
			memset(room_name, 0, ROOM_NAME_MAX_LEN);

			buftodata(1, cmd, 
				CHAR, room_name, offset, ROOM_NAME_MAX_LEN);
			datatobuf(2, buf,
				INT, &code, 0, 4,
				CHAR, room_name, 4, ROOM_NAME_MAX_LEN);
		}
		else if(code == CMD_ROOM_DELETE_CODE) // /delete [room_id]
		{
			int room_id;
			sscanf(cmd+offset, "%d", &room_id);

			datatobuf(2, buf,
				INT, &code, 0, 4,
				INT, &room_id, 4, 4);
		}
		else if(code == CMD_ROOM_CONNECT_CODE) // /connect [room_id]
		{
			int room_id;
			sscanf(cmd+offset, "%d", &room_id);

			datatobuf(2, buf,
				INT, &code, 0, 4,
				INT, &room_id, 4, 4);

			cur_room_id = room_id;
			print_room_title("%s %d", cur_room_name, cur_room_id);
			clear_box_buffer(chat);
			// 오류 처리는 handle_res쪽에서
		}
		else if(code == CMD_ROOM_INVITE_CODE) // /invite [user_id] [room_id]
		{
			int user_id, room_id;
			sscanf(cmd+offset, "%d %d", &user_id, &room_id);

			datatobuf(3, buf,
				INT, &code, 0, 4,
				INT, &user_id, 4, 4,
				INT, &room_id, 8, 4);
		}
		else if(code == CMD_ROOM_LIST_CODE)
		{
			datatobuf(1, buf,
				INT, &code, 0, 4);
		}
		else if(code == CMD_USER_LIST_CODE)
		{
			datatobuf(1, buf,
				INT, &code, 0, 4);
		}
		else if(code == CMD_TOGGLE_HB_LOG)
		{
			heartbeat_log_flag = !heartbeat_log_flag;
			if(heartbeat_log_flag)
				printfBox(lobby, "[*] Heartbeat log: on");
			else
				printfBox(lobby, "[*] Heartbeat log: off");

			send_flag = 0;
		}
		else if(code == CMD_EXIT)
		{
			exit(0);
		}
	}

	move_cursor_cmd();
	for(int i=0 ; i<115 ; i++)
		printf(" ");
	move_cursor_cmd();

	// clear cmd line

	if(send_flag)
	{
		if(DEBUG_FLAG)
		{
			print_info("~>");
			write(fileno(stdout), buf, MAX_REQ_BUF_SIZE);
			print_info("\n");
		}
		write(req_sock, buf, MAX_REQ_BUF_SIZE);
	}
}