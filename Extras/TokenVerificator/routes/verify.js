// Copyright 2019 Xsolla Inc. All Rights Reserved.
// @author Vladimir Alyamkin <ufna@ufna.ru>

let express = require('express');
let router = express.Router();

let jwt = require('jsonwebtoken');

router.post('/', function(req, res, next) {
    res.contentType("application/json");

    let token = req.body.token;
    if(!token) return res.sendStatus(400);

    try {
        let decoded = jwt.verify(token, global.gConfig.login.secret);
        console.log(decoded);

        res.statusCode = 200;

        if(global.gConfig.login.dump_payload) {
            res.write('{"token_payload" : ');
            res.write(JSON.stringify(decoded));
            res.write('}');
        }
        else {
            res.write('{"verified" : true}');
        }

    } catch(err) {
        console.log(err);

        res.statusCode = 422;
        res.write('{"error":{"code":"422","description":"');
        res.write(err.toString());
        res.write('"}}');
    }

    res.end();
});

module.exports = router;
