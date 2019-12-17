// Copyright 2019 Xsolla Inc. All Rights Reserved.
// @author Vladimir Alyamkin <ufna@ufna.ru>

let express = require('express');
let router = express.Router();
let sha1 = require('sha1');

const errorCodes = {
    INVALID_SIGNATURE: 'INVALID_SIGNATURE',
    INVALID_PARAMETER: 'INVALID_PARAMETER'
}

router.post('/', function(req, res, next) {
    // Force connection will be closed
    res.set({
        'Connection': 'close'
    });

    try {
        console.log("Processing payload: " + JSON.stringify(req.body));
        console.log(req);

        global.db.users.findOne({ id: req.body.username }, function (err, doc) {
            if(err)
                requestError(res, err);
            else if(!doc)
                requestError(res, "No user " + req.body.username + " found in test db", errorCodes.INVALID_USER);
            else {
                if(doc.password == req.body.password) {
                    console.log(doc);
                    endRequestAuth(res, req.body.username);
                }
                else {
                    requestError(res, "Invalid user " + req.body.username + " auth data", errorCodes.INVALID_USER);
                }
            }
        });

    } catch(err) {
        requestError(res, err);
    }
});

// Provide status code and finish request processing
endRequestAuth = function (res, token) {
    res.statusCode = 204;
    res.contentType("application/json");
    res.write('{"login_url":"');
    res.write(token.toString());
    res.write('"}}');
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
