import QtQuick 2.0
import Sailfish.Silica 1.0
import Sailfish.OAuth 1.0

ApplicationWindow {
    id: window
    initialPage: Component {
        Page {
            id: page

            Rectangle {
                id: topRect
                width: parent.width
                height: parent.height / 2
                color: "lightsteelblue"

                MouseArea {
                    anchors.fill: parent
                    property bool triggered
                    onClicked: {
                        if (!triggered) {
                            triggered = true
                            twitterOAuth.requestTemporaryToken()
                        }
                    }
                }

                OAuth10a {
                    id: twitterOAuth

                    property var xhr

                    consumerKey: ""     // use your app's consumer_key value
                    consumerSecret: ""  // use your app's consumer_secret value

                    requestTokenEndpoint: "https://api.twitter.com/oauth/request_token"
                    authorizeTokenEndpoint: "https://api.twitter.com/oauth/authorize"
                    accessTokenEndpoint: "https://api.twitter.com/oauth/access_token"

                    onErrorOccurred: console.log("Twitter OAuth1 Error: " + error.code + " = " + error.message + " : " + error.httpCode)

                    onReceivedTemporaryToken: {
                        console.log("Got temporary token: " + oauthToken)
                        authorizeInBrowser(oauthToken, oauthTokenSecret)
                    }
                    onReceivedAccessToken: {
                        console.log("Got access token: " + oauthToken + "; " + oauthTokenSecret)
                        xhr = authenticatedRequest("GET", "https://api.twitter.com/1.1/account/verify_credentials.json",
                                                   oauth1.generateAuthorizationHeader(
                                                           oauthToken, oauthTokenSecret,
                                                           "GET", "https://api.twitter.com/1.1/account/verify_credentials.json"))
                    }
                }
            }
            Rectangle {
                id: bottomRect
                width: topRect.width
                height: topRect.height
                y: topRect.height
                color: "lightgreen"

                MouseArea {
                    anchors.fill: parent
                    property bool triggered
                    onClicked: {
                        if (!triggered) {
                            triggered = true
                            googleOAuth.authorizeInBrowser()
                        }
                    }
                }

                OAuth2AcPkce {
                    id: googleOAuth

                    property var xhr

                    clientId: ""        // use your app's clientId value
                    clientSecret: ""    // use your app's clientSecret value
                    redirectListener.port: 7357 // your app's localhost redirect port.  Not required for Google.

                    scopes: ["https://www.googleapis.com/auth/userinfo.email","https://www.googleapis.com/auth/userinfo.profile"]
                    tokenEndpoint: "https://accounts.google.com/o/oauth2/token"
                    authorizationEndpoint: "https://accounts.google.com/o/oauth2/auth"
                    customParameters: ({ "prompt":"consent" })

                    onErrorOccurred: console.log("Google OAuth2 Error: " + error.code + " = " + error.message + " : " + error.httpCode)

                    onReceivedAuthorizationCode: {
                        console.log("Got auth code, about to request token.")
                        customParameters = {}
                    }
                    onReceivedAccessToken: {
                        console.log("Got access token: " + token.access_token)
                        xhr = authenticatedRequest("GET", "https://www.googleapis.com/oauth2/v2/userinfo",
                                                   "Bearer " + token.access_token)
                    }
                }
            }

            function authenticatedRequest(method, url, authorization) {
                var req = new XMLHttpRequest()
                req.onreadystatechange = function() {
                    if (req.readyState == XMLHttpRequest.DONE) {
                        var response = req.responseText
                        console.log(response)
                    }
                }
                req.open(method, url)
                req.setRequestHeader("Authorization", authorization)
                req.send()
                return req
            }
        }
    }
}

