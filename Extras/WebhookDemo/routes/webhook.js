// Copyright 2019 Xsolla Inc. All Rights Reserved.
// @author Vladimir Alyamkin <ufna@ufna.ru>

let express = require('express');
let router = express.Router();

let jwt = require('jsonwebtoken');

router.post('/', function(req, res, next) {
    try {
        let notification_type = req.body.notification_type;
        if(!notification_type) throw "No notification_type data provided";
        console.log("Processing: " + notification_type + ", payload: " + JSON.stringify(req.body));

        switch (notification_type) {
            case "user_validation":
                console.log("We always trust user exists: " + req.body.user.email);
                break;
            default:
                throw "Unsupported notification_type";
        }

        res.statusCode = 204;
        res.contentType("text/plain");
    } catch(err) {
        console.log(err);

        res.statusCode = 400;
        res.contentType("application/json");
        res.write('{"error":{"code":"' + res.statusCode  + '","description":"');
        res.write(err.toString());
        res.write('"}}');
    }

    res.end();
});

module.exports = router;
