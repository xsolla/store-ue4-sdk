# Changelog

## [1.5.0] - 2023-05-20

### Added
- `AuthenticateViaSocialNetwork` SDK method. It authenticates user via social networks. Method incapsulates web-based and native authentication methods

### Changed
- `FetchPaymentToken` and `FetchCartPaymentToken` SDK methods. Added the `ExternalId` parameter
- Xsolla settings. The `PaymentInterfaceTheme` is converted to `string`
- SDK methods for getting catalog (`UXsollaStoreSubsystem` class). Added the `limits` parameter for items and promotions

### Fixed
- Compilation warnings

## [1.4.0] - 2023-03-31

### Added
- `AuthWithXsollaWidget` SDK method. Allows to open Login widget in the internal browser
- SDK methods for single item purchase (`PurchaseStoreItem` and `PurchaseCurrencyPackage` SDK methods)

### Updated
- Android libraries
- `LaunchPaymentConsole` SDK method. Added `BrowserClosed` callback.
- Project settings. Added:
    - `BuildForSteam` checkbox for project settings. Allows to set up payments via Steam
    - `FacebookClientToken` parameter for project settings. Allows to set up native user authentication via Facebook Android application


### Fixed
- Command line parameters truncation
- Payment UI opening for iOS builds
- Redirects for iOS builds

### Reverted
- `CheckOrder` and `CreateAccountLinkingCode` SDK methods

## [1.3.1] - 2023-02-09

### Fixed
- Undefined URL causing crashes (for certain API calls)

## [1.3.0] - 2023-01-11

### Added
- Native social login for iOS
- Native payments for iOS
- Login by device ID for mobiles to demo
- Deep links for iOS
- SDK methods for ordering free items (`CreateOrderWithSpecifiedFreeItem` and `CreateOrderWithFreeCart` SDK methods)

### Updated
- Separated demo and SDK
- Refinement of the payment status check (launching of the payment UI closing check and limiting short polling time)

### Removed
- Account linking function from demo

## [1.2.0] - 2022-09-02

### Added
- SDK methods for working with subscriptions (`XsollaStoreSubsystem` class)
- Demo user generation

### Changed
- SDK methods for working with catalog (`XsollaStoreSubsystem` class). They support catalog personalization

## [1.0.0] - 2022-07-08

### Added
- Check order status
- Token autorefresh

### Changed
- Xsolla settings
- `XsollaLoginSubsystem` class:
    - `UpdateUserAttribute` SDK method. The method was renamed to `GetUserAttributes`
    - `UpdateUserReadOnlyAttributes` SDK method. The method was renamed to `GetUserReadOnlyAttributes`
    - `UpdateUserDetails` SDK method. The method was renamed to `GetUserDetails`
    - `UpdateUserEmail` SDK method. The method was renamed to `GetUserEmail`
    - `UpdateUserPhoneNumber` SDK method. The method was renamed to `GetUserPhoneNumber`
    - `UpdateFriends` SDK method. The method was renamed to `GetFriends`
    - `UpdateSocialAuthLinks` SDK method. The method was renamed to `GetSocialAuthLinks`
    - `UpdateSocialFriends` SDK method. The method was renamed to `GetSocialFriends`
    - `UpdateUsersFriends` SDK method. The method was renamed to `GetUsersFriends`
    - `UpdateUsersDevices` SDK method. The method was renamed to `GetUsersDevices`
    - `UpdateLinkedSocialNetworks` SDK method. The method was renamed to `GetLinkedSocialNetworks`
- `XsollaInventorySubsystem` class:
    - `UpdateInventory` SDK method. The method was renamed to `GetInventory`
    - `UpdateVirtualCurrencyBalance` SDK method. The method was renamed to `GetVirtualCurrencyBalance`
    - `GetSubscriptions` SDK method. The method was renamed to `GetTimeLimitedItems`
- `XsollaStoreSubsystem` class:
    - `UpdateBundles` SDK method. The method was renamed to `GetBundles`
    - `UpdateGamesList` SDK method. The method was renamed to `GetGamesList`
    - `UpdateVirtualItems` SDK method. The method was renamed to `GetVirtualItems`
    - `UpdateItemGroups` SDK method. The method was renamed to `GetItemGroups`
    - `UpdateVirtualCurrencies` SDK method. The method was renamed to `GetVirtualCurrencies`
    - `UpdateVirtualCurrencyPackages` SDK method. The method was renamed to `GetVirtualCurrencyPackages`

### Fixed
- Minor bugs

### Removed
- JWT authorization
- Data cache
- Following methods:
    - `GetPendingSocialAuthenticationUrl`
    - `GetSocialAccountLinkingHtml`
    - `IsSocialNetworkLinked`

## [0.9.3] - 2021-12-15

### Fixed
- Android SDK initialization error
- `GetGameMode` method call

## [0.9.2] - 2021-12-07

### Fixed
- WebSockets module unloading

## [0.9.1] - 2021-11-29

### Added
- Websocket for purchase status check
- Web shop

### Fixed
- Minor bugs

## [0.9.0] - 2021-09-17

### Changed
- Store demo refactoring

### Fixed
- Issue with displaying embedded browser
- Empty Client ID without explicit plugin initialization

## [0.8.1] - 2021-08-30

### Fixed
- Purchase status polling when buying item with virtual currency
- Categories for editor module properties

## [0.8.0] - 2021-08-25

### Added
- Authentication via phone number and email
- Native authentication via WeChat and QQ on Android
- Dynamic UI theme switching support

### Changed
- Widgets manager usability improvements
- UI themes usability improvements
- User authentication demo refactoring
- Minor demo improvements

### Fixed
- Demo background sizing issue

## [0.7.0] - 2021-06-29

### Added
- Authentication via device ID

### Fixed
- Catalog default locale

### Changed
- Minor demo tweaks
- Plugin settings (Demo Client ID)
- SDK code improvements

## [0.6.4] - 2021-05-07

### Added
- PayStation widget settings
- PayStation redirect policy settings
- New parameters for API methods

### Fixed
- Navigation to external links in embedded browser for Pay Station
- Minor demo issues

## [0.6.3] - 2021-04-23

### Added
- Widgets manager

### Fixed
- Battlepass warnings
- Account linking for Playstation platform
- Demo errors when using custom authentication system
- Google authentication via embedded browser

## [0.6.2] - 2021-04-12

### Changed
- Minor settings improvements

## [0.6.1] - 2021-03-31

### Changed
- Web request analytics logic

## [0.6.0] - 2021-03-29

### Added
- UI themes support
- Battlepass demo
- Xbox One and PS4 support

### Changed
- Mechanism for opening links in web browser
- Web request handling logic

### Fixed
- Purchase status polling

## [0.5.8] - 2021-03-19

### Added
- Web request analytics enhancements

## [0.5.7] - 2020-12-25

### Added
- Native authentication via Facebook and Google on Android
- New gender options

### Changed
- Authentication via access token with custom parameters
- New web store for demo project

## [0.5.6] - 2020-12-17

### Added
- Authorization with access token
- Additional social networks for authorization


## [0.5.5] - 2020-11-26

### Changed
- Inventory functionality in separate module

### Fixed
- Minor bugs
- Code comments

## [0.5.4] - 2020-11-06

### Fixed
- Minor bugs


## [0.5.3] - 2020-10-23

### Added
- Coupon functionality
- Bundle functionality
- Promo codes functionality

### Fixed
- Minor bugs


## [0.5.2] - 2020-09-23

### Added
- User account functionality
- Friends system functionality
- The ability to consume virtual currency when purchasing a virtual item or according to in-game logic


## [0.5.1] - 2020-08-24

### Added
- The support for OAuth 2.0 authorization.
- The ability to get read-only attributes on the client side
- A new setting for encrypting the local cache with user data using the Xsolla Save Encryption Key
- The ability to invalidate JWT


## [0.5.0] - 2020-07-28

### Changed
- Demo maps UI
- Removed deprecated PayStation module


## [0.4.17] - 2020-07-08

### Fixed
- Minor bugs


## [0.4.16] - 2020-06-18

### Added
- Added deep linking configuration for Android builds

### Fixed
- Minor bugs


## [0.4.15] - 2020-05-22

### Added
- The ability to sell non-recurring subscriptions

### Fixed
- Minor bugs


## [0.4.14] - 2020-04-23

### Added
- The ability of signing in via Facebook and Google

### Fixed
- Minor bugs


## [0.4.13] - 2020-04-15

### Added
- The ability of cross-platform account linking for combining player’s inventory on different platforms

### Fixed
- Automatic SDK initialization when using Subsystems
- Minor bugs


## [0.4.12] - 2020-04-08

### Added

- The utility method of getting the Steam session_ticket for authorization
- Possibility of passing JWT to application via terminal for simpler authentication


## [0.4.11] - 2020-03-18

### Fixed

- Minor bugs


## [0.4.10] - 2020-03-17

### Added

- Comments for blueprints

### Fixed

- Minor bugs


## [0.4.9] - 2020-03-06

### Added

- Pay Station theme settings

### Fixed

- Minor bugs



## [0.4.8] - 2020-01-31

### Fixed

- Image loading issues



## [0.4.7] - 2020-01-24

### Added

- Design improvement
- The ability to save token for the Steam autorization

### Fixed

- Minor bugs


## [0.4.6] - 2019-12-23

### Added

- Methods for working with user attributes

### Fixed

- Minor bugs



## [0.4.5] - 2019-12-11

### Added

- The ability to consume items in the inventory demo scene

### Fixed

- Minor bugs
