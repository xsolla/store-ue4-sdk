# Game Commerce plug-in for Unreal Engine 4

Game Commerce plug-in is a set of data structures and methods that you can integrate into your Unity app to work with Xsolla products.

After integration, you can use:

*   [Login](https://developers.xsolla.com/doc/login/) for authenticating users and managing the friend system and user account
*   [In-Game Store](https://developers.xsolla.com/doc/in-game-store/) for managing in-game purchases and player inventory in your app
*   [Pay Station](https://developers.xsolla.com/doc/pay-station/) for setting up payments

![Game Commerce demo](https://i.imgur.com/cx0YR1r.png "Game Commerce demo")

We recommend using the game Commerce plug-in as is. For some specific situations, you can use other Xsolla plug-ins:

*   Use [Login & Account System plug-in](https://www.unrealengine.com/marketplace/en-US/product/xsolla-login-sdk) if you are interested in the ready-made login system only.
*   Use [Cross-Buy plug-in](https://www.unrealengine.com/marketplace/en-US/product/player-inventory-sdk) if you are going to publish your app on a platform that restricts the use of third-party payment tools.

<div style="background-color: WhiteSmoke">
<p><b>Note:</b> Game Commerce plug-in includes Login & Account System and Cross-Buy plug-ins. You do <b>not</b> need to install them separately.</p></div>

For a better understanding of which plug-in to choose, use the table:


<table>
  <tr>
   <td>
   </td>
   <td style="text-align: center"><b>Game Commerce plug-in</b>
   </td>
   <td style="text-align: center"><b>Login & Account System plug-in</b>
   </td>
   <td><b>Cross-Buy plug-in</b>
   </td>
  </tr>
  <tr>
   <td colspan="4" ><strong>In-game store</strong>
   </td>
  </tr>
  <tr>
   <td>
    Virtual currency
   </td>
   <td>+
   </td>
   <td>
   </td>
   <td>
   </td>
  </tr>
  <tr>
   <td>
    Virtual items
   </td>
   <td>+
   </td>
   <td>
   </td>
   <td>
   </td>
  </tr>
  <tr>
   <td>
    Player inventory
   </td>
   <td>+
   </td>
   <td>
   </td>
   <td>+
   </td>
  </tr>
  <tr>
   <td>
    Bundles
   </td>
   <td>+
   </td>
   <td>
   </td>
   <td>
   </td>
  </tr>
  <tr>
   <td>
    Promotional campaigns
   </td>
   <td>+
   </td>
   <td>
   </td>
   <td>
   </td>
  </tr>
  <tr>
   <td colspan="4" ><strong>Login</strong>
   </td>
  </tr>
  <tr>
   <td>
    Authentication
   </td>
   <td>+
   </td>
   <td>+
   </td>
   <td>+
   </td>
  </tr>
  <tr>
   <td>
    User management
   </td>
   <td>+
   </td>
   <td>+
   </td>
   <td>+
   </td>
  </tr>
  <tr>
   <td><strong>Payment UI</strong>
   </td>
   <td>+
   </td>
   <td>
   </td>
   <td>
   </td>
  </tr>
  <tr>
   <td colspan="4" ><strong>Additional features</strong>
   </td>
  </tr>
  <tr>
   <td>
    UI builder
   </td>
   <td>
    +
   </td>
   <td>
    +
   </td>
   <td>
    +
   </td>
  </tr>
  <tr>
   <td>
    Battle pass
   </td>
   <td>
    +
   </td>
   <td>
   </td>
   <td>
   </td>
  </tr>
</table>



## Requirements


### System requirements

*   64-bit OS
*   Windows 7 and later
*   macOS 10.13.6 and later
*   XCode 11.1 and later (for macOS)
*   Visual Studio 2017 and later
*   Unreal Engine 4.25 and later


### Target OS

*   iOS
*   Android
*   macOS
*   Windows 64-bit

<div style="background-color: WhiteSmoke">
<p><b>Note:</b> To modify the SDK for your application specifics, follow the <a href="https://developers.xsolla.com/sdk/unreal-engine/how-tos/other/#unreal_engine_sdk_how_to_modify_sdk">SDK modification instruction</a>.</p></div>


# Integration

The following integration options are available:


<table>
  <tr>
   <td>
<h3  style="text-align: center">
<img src="https://i.imgur.com/3bdXcuv.png" width="50" >
</h3>
<h3>Use Xsolla servers</h3>
   </td>
   <td>
<h3 style="text-align: center">
<img src="https://i.imgur.com/eGmKMPX.png" width="50" >
</h3>
<h3>Use your server</h3>
   </td>
  </tr>
  <tr>
   <td>Choose this option if you want a ready-made server solution for monetization and in-game items management. After integration of the plug-in, you can use <a href="https://developers.xsolla.com/doc/login/">Xsolla Login</a>, <a href="https://developers.xsolla.com/doc/in-game-store/">In-Game Store</a>, <a href="https://developers.xsolla.com/doc/in-game-store/features/player-inventory/">Player Inventory</a>, and <a href="https://developers.xsolla.com/doc/pay-station/">Xsolla Pay Station</a>.
   </td>
   <td>Choose this option if you have already implemented the game logic for authentication, in-game store, and player inventory on your servers and want to use <a href="https://developers.xsolla.com/doc/pay-station/">Xsolla Pay Station</a>.
   </td>
  </tr>
  <tr>
   <td><a href="https://developers.xsolla.com/sdk/unreal-engine/commerce/use-xsolla-servers/">Get started →</a> 
   </td>
   <td><a href="https://developers.xsolla.com/sdk/unreal-engine/commerce/use-your-server-side/">Get started →</a>
   </td>
  </tr>
</table>


## Usage 

Xsolla provides APIs to work with it’s products. The Game Commerce plug-in provides classes and methods for API calls, so you won’t need to write boilerplate code. Use the [tutorials](https://developers.xsolla.com/sdk/unreal-engine/tutorials/) to learn how you can use the [plug-in methods](https://developers.xsolla.com/sdk-code-references/unreal-store/).

## Legal info

[Explore legal information](https://developers.xsolla.com/sdk/unreal-engine/commerce/get-started/#sdk_legal_compliance) that helps you work with Xsolla.

Xsolla offers the necessary tools to help you build and grow your gaming business, including personalized support at every stage. The terms of payment are determined by the contract that can be signed via Publisher Account.

**The cost of using all Xsolla products is 5% of the amount you receive for the sale of the game and in-game goods via Xsolla Pay Station**. If you do not use Xsolla Pay Station in your application, but use other products, contact your Account Manager to clarify the terms and conditions.

---

### License

See the [LICENSE](https://github.com/xsolla/store-ue4-sdk/blob/master/LICENSE.txt) file.


### Community

[Join our Discord server](https://discord.gg/auNFyzZx96) and connect with the Xsolla team and developers who use Xsolla products.


### Additional resources

*   [Xsolla official website](https://xsolla.com/)
*   [Developers documentation](https://developers.xsolla.com/sdk/unreal-engine/)
*   [Code reference](https://developers.xsolla.com/sdk-code-references/unreal-store/)
*   API reference:
    *   [Pay Station API](https://developers.xsolla.com/pay-station-api/)
    *   [Login API](https://developers.xsolla.com/login-api/) 
    *   [Commerce API](https://developers.xsolla.com/commerce-api/)