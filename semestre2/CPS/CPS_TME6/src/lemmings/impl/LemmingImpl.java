package lemmings.impl;

import lemmings.services.GameEngService;
import lemmings.services.LemmingService;
import lemmings.services.RequiredServiceGameEng;

public class LemmingImpl implements
		/* require */
		RequiredServiceGameEng,
		/* provide */
		LemmingService {

	@Override
	public void bindServiceGameEng(GameEngService ges) {
		// TODO Auto-generated method stub

	}

}
