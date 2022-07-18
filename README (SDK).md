
The SDK makes it easier to integrate Xsolla products into your Unreal Engine project by providing out-of-the-box data structures and methods for working with Xsolla API.

After integration, you can use:

* [Login](https://developers.xsolla.com/doc/login/) to authenticate users and manage the friend system and user account
* [In-Game Store](https://developers.xsolla.com/doc/in-game-store/) for managing in-game purchases and player inventory in your application.
* [Pay Station](https://developers.xsolla.com/doc/pay-station/) for setting up payments.

[Learn more about supported features →](#Features)

To start with the SDK, you need to install this plug-in and set up a project in [Xsolla Publisher Account](https://publisher.xsolla.com/signup?store_type=sdk).

[Go to the integration guide →](https://developers.xsolla.com/sdk/unreal-engine/integrate-complete-solution/)


## Features

### Authentication

* [OAuth 2.0](https://oauth.net/2/) protocol-based authentication.
* Classic login (username/email and password).
* Social login.
* Social login on the user’s device via a social network client.
* Login via a launcher (Steam or [Xsolla Launcher](https://developers.xsolla.com/doc/launcher/)).
* Login via a device ID.
* Passwordless login via a one-time code or a link sent via SMS or email.


### User management

* User attributes to manage additional information.
* Friend system.
* Cross-platform account linking.
* Secure Xsolla storage for user data. Alternatively, you can connect PlayFab, Firebase, or your custom storage.

**INFO:** You can also manage user accounts and access rights via Xsolla Publisher Account.

### Catalog

* Virtual currency
    * Sell virtual currency in any amount or in packages (for real money or other virtual currency).
    * Sell hard currency (for real money only).
* Virtual items:
    * Set up a catalog of in-game items.
    * Sell virtual items for real and virtual currency.
* Bundles:
    * Sell bundles for real or virtual currency.
* Promotional campaigns:
    * Reward users with virtual currency packages, game keys, or virtual items for coupons.
    * Give users bonuses or apply a discount on items in the cart with promo codes.

**INFO:** You can add items in the catalog or manage campaigns with discounts, coupons, and promo codes via Xsolla Publisher Account.


### Item purchase

- Sell items in one click or via the shopping cart.
- Provide users with a convenient payment UI. The main features are:
	- 700+ payment methods in 200+ countries, including bank cards, digital wallets, mobile payments, cash kiosks, gift cards, and special offers.
	- 130+ currencies.
	- UI localized into 20+ languages.
	- Desktop and mobile versions.

**INFO:** Xsolla Publisher Account provides you with purchase analytics, transaction history, and other statistics.

### Player inventory

* Get and verify an inventory.
* Consume items according to the game logic.
* Consume virtual currency according to the in-game logics (for example, when opening a location or purchasing level for some currency).
* Synchronize all purchases and premium rewards of the user across all platforms.


## Requirements

### System requirements

* 64-bit OS
* Windows 7 and later
* macOS 10.13.6 and later
* XCode 11.1 and later (for macOS)
* Visual Studio 2017 and later
* Unreal Engine 4.25 and later

### Target OS

* iOS
* Android
* macOS
* Windows 64-bit

**NOTE:** To modify the SDK for your application specifics, follow the [SDK modification instruction](https://developers.xsolla.com/sdk/unreal-engine/usage/how-to-modify-sdk/).

## Usage

To send requests to Xsolla servers and receive responses, the SDK provides the `XsollaStoreSubsystem`, `XsollaLoginSubsystem`, and `XsollaInventorySubsystem` classes. If you want to implement your own logic for buying items or inventory management, and don’t want to write boilerplate code for API calls, these classes are a good place to start.

Follow the [step-by-step tutorials](https://developers.xsolla.com/sdk/unreal-engine/integrate-complete-solution/integrate-on-app-side/) to get started with basic SDK features.

Explore [code reference documentation](https://developers.xsolla.com/sdk-code-references/unreal-store/) to learn more about SDK methods.


## Pricing

Xsolla offers the necessary tools to help you build and grow your gaming business, including personalized support at every stage. The terms of payment are determined by the contract that you can sign via Publisher Account.

The cost of using all Xsolla products is 5% of the amount you receive for the sale of the game and in-game goods via Xsolla Pay Station. If you don’t use Xsolla Pay Station in your application, but use other products, contact your Account Manager to clarify the terms and conditions.

Explore [legal information](https://xsolla.com/pricing) that helps you work with Xsolla.

## License

See the [LICENSE](https://github.com/xsolla/store-ue4-sdk/blob/master/LICENSE.txt) file.

## Contacts


* [Support team and feedback](https://xsolla.com/partner-support)
* [Integration team](mailto:integration@xsolla.com)


## Additional resources

* [Xsolla official website](https://xsolla.com/)
* [Developers documentation](https://developers.xsolla.com/sdk/unreal-engine/)
* [Code reference documentation](https://developers.xsolla.com/sdk-code-references/unreal-store/)