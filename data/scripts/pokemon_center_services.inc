PokemonCenter_EventScript_PokemonCenterServices::
	lock
	faceplayer
	msgbox PokemonCenter_Text_PokemonCenterServicesIntro, MSGBOX_DEFAULT
	multichoice 0, 0, MULTI_POKEMON_CENTER_SERVICES, FALSE
	switch VAR_RESULT
	case 0, PokemonCenter_EventScript_ChangeNature
	case 1, PokemonCenter_EventScript_ChangeGender
	release
	end

PokemonCenter_EventScript_ChangeNature::
	msgbox PokemonCenter_Text_NotImplemented, MSGBOX_DEFAULT
	release
	end

PokemonCenter_EventScript_ChangeGender::
	msgbox PokemonCenter_Text_NotImplemented, MSGBOX_DEFAULT
	release
	end

PokemonCenter_Text_PokemonCenterServicesIntro:
	.string "Hello there, what can I\n"
	.string "help you with today?$"

PokemonCenter_Text_NotImplemented:
	.string "Coming soon!$"