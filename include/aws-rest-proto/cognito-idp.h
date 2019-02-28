#pragma once
#include "request_response_lang.h"

namespace arp {

using cognito = decltype(service("AWSCognitoIdentityProviderService"_s, "cognito-idp"_s));
using challenge_password_verify =                         //
    decltype(response<cognito>("InitiateAuthResponse"_s,  //
                               payload(ensure("ChallengeName"_s, "PASSWORD_VERIFIER"_s),
                                       object("ChallengeParameters"_s,               //
                                              param<std::string>("SECRET_BLOCK"_s),  //
                                              param<std::string>("SRP_B"_s),
                                              param<std::string>("USER_ID_FOR_SRP"_s),  //
                                              param<std::string>("SALT"_s)))));         // Suggested reply -> respond to auth challenge
using challenge_sms_mfa =                                                               //
    decltype(response<cognito>("InitiateAuthResponse"_s,                                //
                               payload(ensure("ChallengeName"_s, "SMS_MFA"_s))));

using auth_token =               //
    decltype(response<cognito>(  //
        "InitiateAuthResponse"_s,
        payload(     //
            object(  //
                "AuthenticationResult"_s,
                param<std::string>("AccessToken"_s),  //
                param<uint64_t>("ExpiresIn"_s),       //
                param<std::string>("IdToken"_s),      //
                param<std::string>("RefreshToken"_s)))));

using srp_challenge =                                                //
    decltype(request<cognito>("RespondToAuthChallenge"_s,            //
                              payload(                               //
                                  param<std::string>("ClientId"_s),  //
                                  ensure("ChallengeName"_s, "PASSWORD_VERIFIER"_s),
                                  object("ChallengeResponses"_s,                               //
                                         param<std::string>("PASSWORD_CLAIM_SECRET_BLOCK"_s),  //
                                         param<std::string>("PASSWORD_CLAIM_SIGNATURE"_s),     //
                                         param<std::string>("USERNAME"_s),                     //
                                         param<std::string>("TIMESTAMP"_s)                     //
                                         )),                                                   //
                              error_response(                                                  //
                                  "NotAuthorizedException"_s,                                  //
                                  "InvalidPasswordException"_s,
                                  "PasswordResetRequiredException"_s,
                                  any_other                                                    //
                                  ),
                              response_ref<auth_token>()));

using sms_mfa_challenge =                                                         //
    decltype(request<cognito>("RespondToAuthChallenge"_s,                         //
                              payload(                                            //
                                  ensure("ChallengeName"_s, "SMS_MFA"_s),         //
                                  param<std::string>("ClientId"_s),               //
                                  object("ChallengeResponses"_s,                  //
                                         param<std::string>("USERNAME"_s),        //
                                         param<std::string>("SMS_MFA_CODE"_s))),  //
                              error_response(                                     //
                                  "NotAuthorizedException"_s,                     //
                                  any_other                                       //
                                  ),
                              response_ref<auth_token>()));

using user_srp_auth =                                             //
    decltype(request<cognito>("InitiateAuth"_s,                   //
                              error_response(                     //
                                  "UserNotConfirmedException"_s,  //
                                  "UserNotFoundException"_s,      //
                                  "InvalidPasswordException"_s,      //
                                  any_other),                     //
                              payload(ensure("AuthFlow"_s, "USER_SRP_AUTH"_s),
                                      object("AuthParameters"_s,                 //
                                             param<std::string>("SRP_A"_s),      //
                                             param<std::string>("USERNAME"_s)),  //
                                      param<std::string>("ClientId"_s)),
                              response_ref<challenge_password_verify>()  //
                              ));

using refresh_token =                                             //
    decltype(request<cognito>("InitiateAuth"_s,                   //
                              error_response(                     //
                                  "UserNotConfirmedException"_s,  //
                                  "UserNotFoundException"_s,      //
                                  any_other),                     //
                              payload(ensure("AuthFlow"_s, "REFRESH_TOKEN"_s),
                                      object("AuthParameters"_s,                      //
                                             param<std::string>("REFRESH_TOKEN"_s)),  //
                                      param<std::string>("ClientId"_s)),
                              response_ref<auth_token>()  //
                              ));
}  // namespace arp
