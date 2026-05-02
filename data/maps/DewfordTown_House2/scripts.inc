DewfordTown_House2_MapScripts::
	.byte 0

DewfordTown_House2_EventScript_Man::
	lock
	faceplayer
	goto_if_set FLAG_RECEIVED_PUNCHING_GLOVE, DewfordTown_House2_EventScript_ExplainPunchingGlove
	msgbox DewfordTown_House2_Text_WantYouToHavePunchingGlove, MSGBOX_DEFAULT
	giveitem ITEM_PUNCHING_GLOVE
	goto_if_eq VAR_RESULT, FALSE, DewfordTown_House2_EventScript_NoRoomForScarf
	setflag FLAG_RECEIVED_PUNCHING_GLOVE
	release
	end

DewfordTown_House2_EventScript_NoRoomForScarf::
	msgbox DewfordTown_House2_Text_NoRoom, MSGBOX_DEFAULT
	release
	end

DewfordTown_House2_EventScript_ExplainPunchingGlove::
	msgbox DewfordTown_House2_Text_ExplainPunchingGlove, MSGBOX_DEFAULT
	release
	end

DewfordTown_House2_EventScript_Boy::
	msgbox DewfordTown_House2_Text_BrawlySoCool, MSGBOX_NPC
	end

DewfordTown_House2_Text_WantYouToHavePunchingGlove:
	.string "Gorge your eyes on this!\p"
	.string "It's a PUNCHING GLOVE. The ultimate\n"
	.string "gear for any fighting trainer, yeah!\p"
	.string "Oh, I can see your eyes twinkling!\n"
	.string "You appreciate my impeccable taste!\p"
	.string "Oh, you're a delight!\n"
	.string "Here you go. I want you to have it!$"

DewfordTown_House2_Text_NoRoom:
	.string "Oh, you don't have room?\p"
	.string "Now, listen tight, this PUNCHING GLOVE\n"
	.string "is a must-have! Why, I would sell all\l"
	.string "my items in order to get it!$"

DewfordTown_House2_Text_ExplainPunchingGlove:
	.string "The PUNCHING GLOVE raises the power\n"
	.string "of punching moves.\p"
	.string "It also prevents contact effects\n"
	.string "from triggering!$"

DewfordTown_House2_Text_BrawlySoCool:
	.string "Wow, you bothered to cross the sea\n"
	.string "to visit DEWFORD?\p"
	.string "Did you maybe come here because you\n"
	.string "heard about BRAWLY?\p"
	.string "He's so cool…\n"
	.string "Everyone idolizes him.$"

