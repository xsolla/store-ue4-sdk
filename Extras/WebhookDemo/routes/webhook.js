// Copyright 2019 Xsolla Inc. All Rights Reserved.
// @author Vladimir Alyamkin <ufna@ufna.ru>

let express = require('express');
let router = express.Router();

let jwt = require('jsonwebtoken');

router.post('/', function(req, res, next) {
    res.contentType("application/json");

    let notification_type = req.body.notification_type;
    if(!notification_type) return res.sendStatus(400);

    try {
        console.log("Here we go again! " + notification_type);

        res.statusCode = 204;
        res.write('{"verified" : true}');

    } catch(err) {
        console.log(err);

        res.statusCode = 400;
        res.write('{"error":{"code":"422","description":"');
        res.write(err.toString());
        res.write('"}}');
    }

    res.end();
});

module.exports = router;
