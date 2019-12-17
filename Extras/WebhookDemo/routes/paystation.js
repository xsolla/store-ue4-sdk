// Copyright 2019 Xsolla Inc. All Rights Reserved.
// @author Vladimir Alyamkin <ufna@ufna.ru>

let express = require('express');
let router = express.Router();

router.post('/', function(req, res, next) {
    res.contentType("application/json");

    let url = 'https://'
        + global.gConfig.paystation.merchantId + ':' + global.gConfig.paystation.apiKey
        + '@api.xsolla.com/merchant/v2/merchants/'
        + global.gConfig.paystation.merchantId
        + '/token';

    // prepare
    let tokenJson = require('./token.json');
    tokenJson.settings.project_id = this.globals.projectId;
    tokenJson.user.id.value = req.raw.body;

    let options = {
        url: url,
        method: 'POST',
        headers: {
            'Content-Type': 'application/json',
            'Content-Length': tokenJson.length
        },
        json: tokenJson
    };

    try {
        makeRequest(options, (err, tokenRes, body) => {
            if (err) throw err;

            if (tokenRes && body.token) {
                res.statusCode = 200;
                res.end(body.token);
            } else {
                res.statusCode = 422;
                res.end();
            }
        });

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
