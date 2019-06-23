// Copyright 2019 Xsolla Inc. All Rights Reserved.
// @author Vladimir Alyamkin <ufna@ufna.ru>

let express = require('express');
let router = express.Router();
let sha1 = require('sha1');

let jwt = require('jsonwebtoken');

router.post('/', function(req, res, next) {
    try {
        let notification_type = req.body.notification_type;
        if(!notification_type) throw "No notification_type data provided";
        console.log("Processing: " + notification_type + ", payload: " + JSON.stringify(req.body));

        // Check request signature
        let serverSignature = "Signature " + sha1(JSON.stringify(req.body) + global.gConfig.store.secretKey);
        if(serverSignature != req.headers.authorization)
        {
            throw "Invalid Signature. Signature provided in \"Authorization\" header does not match with expected"
        }

        // Route request to desired notification handler
        switch (notification_type) {
            case "user_validation":
                // Check user existence
                global.db.users.findOne({ id: req.body.user.id }, function (err, doc) {
                    if(err)
                        requestError(res, err);
                    else if(!doc)
                        requestError(res, "No user " + req.body.user.id + " found in test db");
                    else {
                        console.log(doc);
                        endRequest(res);
                    }
                });
                break;
            case "payment":
                throw notification_type + " is not supported yet"
                break;
            case "refund":
                throw notification_type + " is not supported yet"
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

requestError = function (res, err) {
    console.log(err);

    res.statusCode = 400;
    res.contentType("application/json");
    res.write('{"error":{"code":"' + res.statusCode  + '","description":"');
    res.write(err.toString());
    res.write('"}}');
    res.end();
}

module.exports = router;
