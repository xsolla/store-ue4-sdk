// Copyright 2019 Xsolla Inc. All Rights Reserved.
// @author Vladimir Alyamkin <ufna@ufna.ru>

const config = require('./config.json');
global.gConfig = config;

let express = require('express');
let logger = require('morgan');

let webhookRouter = require('./routes/webhook');

let app = express();

app.use(logger('dev'));
app.use(express.json());
app.use(express.urlencoded({ extended: false }));

app.use('/webhook', webhookRouter);

module.exports = app;
