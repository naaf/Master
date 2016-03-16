package lemmings.impl;

import lemmings.services.GameEngService;
import lemmings.services.LevelService;
import lemmings.services.RequiredServiceLevel;

public class GameEngImpl implements
		/* require */
		RequiredServiceLevel,
		/* provide */
		GameEngService {

	@Override
	public void bindLevelService(LevelService ls) {
		// TODO Auto-generated method stub

	}

}
