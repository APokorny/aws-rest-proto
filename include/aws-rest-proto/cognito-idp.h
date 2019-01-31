#pragma once
#include "request_response_lang.h"

namespace arp {

using cognito = decltype(service("AWSCognitoIdentityProviderService"_s));
using initiate_auth_response = decltype(response<cognito>("InitiateAuthResponse"_s, //
        payload(
            param<std::string>("ChallengeNameType"_s),
            object("ChallengeParameters"_s, //
                param<std::string>("SECRET_BLOCK"_s),
                param<std::string>("SRP_B"_s),
                param<std::string>("USER_ID_FOR_SRP"_s),
                param<std::string>("SALT"_s),
            )
            ))); // Suggested reply -> respond to auth challenge
using respond_to_auth_challenge = decltype(request<cognito>("RespondToAuthChallenge"_s,                                      //
                                                            payload(                                                         //
                                                                param<std::string>("ClientId"_s),                            //
                                                                param<std::string>("ChallengeName"_s),                       //
                                                                object("ChallengeResponses"_s                                //
                                                                       param<std::string>("PASSWORD_CLAIM_SECRET_BLOCK"_s),  //
                                                                       param<std::string>("PASSWORD_CLAIM_SIGNATURE"_s),     //
                                                                       param<std::string>("USERNAME"_s),                     //
                                                                       param<std::string>("TIMESTAMP"_s)                     //
                                                                       )),                                                   //
                                                            error_response(                                                  //
                                                                "InvalidPasswordException"_s,                                //
                                                                "UserNotFoundException"_s,                                   //
                                                                "PasswordResetRequiredException"_s,                          //
                                                                any_other                                                    //
                                                                )));

using initiate_auth = decltype(request<cognito>("InitiateAuth"_s,                               //
                                                error_reponse(                                  //
                                                    "UserNotConfirmedException"_s,              //
                                                    "UserNotFoundException"_s,                  //
                                                    any_other,                                  //
                                                    ),                                          //
                                                payload response_ref<initiate_auth_response>()  //
                                                ));
}  // namespace arp
