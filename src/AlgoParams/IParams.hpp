//============================================================================
// Name        	: IParams.hpp
// Author      	: Sam Gaer
// Version     	:
// Copyright   	: Copyright (C) 2017-2023 Katana Financial
// Date			: Dec 22, 2023 10:56:22 AM
//============================================================================

#ifndef SRC_ALGOPARAMS_IPARAMS_HPP_
#define SRC_ALGOPARAMS_IPARAMS_HPP_


struct IParams
{
	virtual void Print();
};

struct IOBParams
{
	virtual ~IOBParams() {}
	int paramindex;
};

#endif /* SRC_ALGOPARAMS_IPARAMS_HPP_ */
