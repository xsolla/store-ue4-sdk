// Copyright 2019 Xsolla Inc. All Rights Reserved.
// @author Vladimir Alyamkin <ufna@ufna.ru>

let express = require('express');
let router = express.Router();
let sha1 = require('sha1');

let jwt = require('jsonwebtoken');

const errorCodes = {
    INVALID_USER: 'INVALID_USER',
    INVALID_PARAMETER: 'INVALID_PARAMETER',
    INVALID_SIGNATURE: 'INVALID_SIGNATURE',
    INCORRECT_AMOUNT: 'INCORRECT_AMOUNT',
    INCORRECT_INVOICE: 'INCORRECT_INVOICE'
}

router.post('/', function(req, res, next) {
    // Force connection will be closed
    res.set({
        'Connection': 'close'
    });

    try {
        let notification_type = req.body.notification_type;
        if(!notification_type) throw "No notification_type data provided";
        console.log("Processing: " + notification_type + ", payload: " + JSON.stringify(req.body));

        // Check request signature
        let serverSignature = "Signature " + sha1(JSON.stringify(req.body) + global.gConfig.store.secretKey);
        if(serverSignature != req.headers.authorization)
        {
            requestError(res, "Invalid signature.", errorCodes.INVALID_SIGNATURE);
            return;
        }

        // Route request to desired notification handler
        switch (notification_type) {
            case "user_validation":
                if(global.gConfig.store.validateUser) {
                    global.db.users.findOne({ id: req.body.user.id }, function (err, doc) {
                        if(err)
                            requestError(res, err);
                        else if(!doc)
                            requestError(res, "No user " + req.body.user.id + " found in test db", errorCodes.INVALID_USER);
                        else {
                            console.log(doc);
                            endRequest(res);
                        }
                    });
                }
                else {
                    console.log("Don't validate user and trust him: " + req.body.user.id);
                    endRequest(res);
                }
                break;

            case "payment":
                global.db.purchases.insert({transaction: req.body.transaction.id, user: req.body.user.id, seen: false}, function (err, newDoc) {
                    if(err)
                        requestError(res, err);
                    else {
                        console.log(newDoc);
                        endRequest(res);
                    }
                });
                break;

            case "refund":
                // Just say "all is okay" for demo purposes
                endRequest(res);
                break;

            default:
                throw "Unsupported notification_type";
        }

    } catch(err) {
        requestError(res, err);
    }
});

// Provide status code and finish request processing
endRequest = function (res) {
    res.statusCode = 204;
    res.contentType("text/plain");
    res.end();
}

requestError = function (res, err, errorCode = errorCodes.INVALID_PARAMETER) {
    console.log(err);

    res.statusCode = 400;
    res.contentType("application/json");
    res.write('{"error":{"code":"' + errorCode  + '","description":"');
    res.write(err.toString());
    res.write('"}}');
    res.end();
}

module.exports = router;
