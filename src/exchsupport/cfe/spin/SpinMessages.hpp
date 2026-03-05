/*
 * SpinMessages.hpp
 *
 *  Created on: Dec 8, 2017
 *      Author: sgaer
 */

#ifndef CFE_SPIN_MESSAGES_HPP_
#define CFE_SPIN_MESSAGES_HPP_

#include <stdio.h>
#include <sys/types.h>
#include <iostream>
#include <vector>
#include <map>

#include <KT01/Core/StringHelpers.hpp>
#include <exchsupport/cfe/pitch/PitchMsgTypes.hpp>
#include <string.h>

namespace KTN
{
    namespace CFE
    {
        namespace Spin
        {
            struct SequencedUnitHeader{
                uint16_t 	HdrLength;
                uint8_t 	HdrCount;
                uint8_t 	HdrUnit;
                uint32_t	HdrSequence;
            }__attribute__((packed));

            struct LoginMsg
            {
                SequencedUnitHeader hdr;
                uint8_t Length;
                uint8_t MsgType;
                char SessionSubId[4];
                char Username[4];
                char Filler[2];
                char Password[10];

                void Construct(string username, string password, string session)
                {
                    hdr.HdrLength = sizeof(LoginMsg);// + sizeof(SequencedUnitHeader);
                    hdr.HdrCount = 1;
                    hdr.HdrUnit = 0;
                    hdr.HdrSequence = 0;
        
                    Length = sizeof(LoginMsg) - sizeof(SequencedUnitHeader);
                    MsgType = 0x01;

                   // Use ToAlphanumeric to populate fields
                    char* s1 = StringHelpers::ToAlphanumeric(session, sizeof(SessionSubId));
                    std::memcpy(SessionSubId, s1, sizeof(SessionSubId));
                    delete[] s1;

                    char* s2 = StringHelpers::ToAlphanumeric(username, sizeof(Username));
                    std::memcpy(Username, s2, sizeof(Username));
                    delete[] s2;

                    char* s3 = StringHelpers::ToAlphanumeric("  ", sizeof(Filler));  // fixed 2 spaces
                    std::memcpy(Filler, s3, sizeof(Filler));
                    delete[] s3;

                    char* s4 = StringHelpers::ToAlphanumeric(password, sizeof(Password));
                    std::memcpy(Password, s4, sizeof(Password));
                    delete[] s4;
                }

            } __attribute__((packed));
            

            struct LoginResponseMsg
            {
                //SequencedUnitHeader hdr;
                uint8_t Length;
                uint8_t MsgType;
                char Status;
            } __attribute__((packed));

            struct SpinImageAvailableMsg
            {
                //SequencedUnitHeader hdr;
                uint8_t Length;
                uint8_t MsgType;
                uint32_t Sequence;
            } __attribute__((packed));

            struct SpinRequestMsg
            {
                SequencedUnitHeader hdr;
                uint8_t Length;
                uint8_t MsgType;
                uint32_t Sequence;

                void Construct(uint32_t sequence)
                {
                    hdr.HdrLength = sizeof(SpinRequestMsg);
                    hdr.HdrCount = 1;
                    hdr.HdrUnit = 0;
                    hdr.HdrSequence = 0;

                    Length = sizeof(SpinRequestMsg) - sizeof(SequencedUnitHeader);
                    MsgType = EnumSpinRequest;
                    Sequence = sequence;
                }
            } __attribute__((packed));

            struct SpinResponseMsg
            {
                //SequencedUnitHeader hdr;
                uint8_t Length;
                uint8_t MsgType;
                uint32_t Sequence;
                uint32_t OrderCount;
                EnumSpinStatus Status;
            } __attribute__((packed));

            struct SpinFinishedMsg
            {
                //SequencedUnitHeader hdr;
                uint8_t Length;
                uint8_t MsgType;
                uint32_t Sequence;
            } __attribute__((packed));

            //------------------------
        } // namespace SpinServer
    } // namespace CFE
} // namespace KTN

#endif /* CFE_SPIN_MESSAGES_HPP_ */