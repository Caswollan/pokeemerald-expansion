
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
	msgbox PokemonCenter_Text_ChoosePokemon, MSGBOX_DEFAULT
	special ChoosePartyMon
	waitstate
	goto_if_ge VAR_0x8004, PARTY_SIZE, PokemonCenter_EventScript_ChangeNatureCancel
	special OpenNatureMenu
	waitstate
	goto_if_eq VAR_RESULT, 25, PokemonCenter_EventScript_ChangeNatureCancel
	special SetHiddenNature
	msgbox PokemonCenter_Text_NatureChanged, MSGBOX_DEFAULT
	release
	end

PokemonCenter_EventScript_ChangeNatureCancel::
	release
	end

PokemonCenter_EventScript_ChangeGender::
	msgbox PokemonCenter_Text_NotImplemented, MSGBOX_DEFAULT
	release
	end

PokemonCenter_Text_PokemonCenterServicesIntro:
	.string "Hello there, what can I help you\n"
	.string "with today?$"

PokemonCenter_Text_ChoosePokemon:
	.string "Which POKéMON would you like to change?$"

PokemonCenter_Text_NatureChanged:
	.string "Done! Your POKéMON's nature has been\n"
	.string "changed!$"

PokemonCenter_Text_NotImplemented:
	.string "Coming soon!$"