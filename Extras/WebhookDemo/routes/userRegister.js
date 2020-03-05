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

        global.db.users.insert({id: req.body.username, email: req.body.email, password: req.body.password}, function (err, newDoc) {
            if(err)
                requestError(res, err);
            else {
                console.log(newDoc);
                endRequest(res);
            }
        });

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
