# SDK composition

The SDK consists of the following parts:

*   The main SDK module — a set of classes that are data structures and methods for making requests to the Xsolla REST APIs. The main SDK module includes:
    *   Methods and classes for working with user authentication, the friend system, obtaining user information, etc.
    *   Methods and data structures for working with virtual items and virtual currencies, making purchases, payment processing, managing a shopping cart, opening the payment UI, etc.
    *   Methods and classes for managing the player's inventory and the virtual currency balance.
*   Demo project module — a set of blueprints and assets to demonstrate how the SDK works.


## The main SDK module


### SDK methods for calling the Xsolla APIs

To send requests to Xsolla servers and receive responses, the SDK provides the `XsollaStoreSubsystem`, `XsollaLoginSubsystem` and `XsollaInventorySubsystem` classes. If you want to implement your own logic for buying items or inventory management, and don’t want to write boilerplate code for API calls, these classes are a good place to start.

The `XsollaLoginSubsystem` class includes methods to implement the following features:
*   sign-up
*   email confirmation
*   user authentication via various mechanisms
*   password reset
*   cross-platform account linking
*   managing user attributes
*   friend system
*   user account

The `XsollaStoreSubsystem` class includes methods to implement the following features:
*   selling virtual items and virtual currency
*   managing an in-game store
*   managing a shopping cart
*   working with promotional campaigns (discounts, coupons, and promo codes)

The `XsollaInventorySubsystem `class includes methods to implement the following features:
*   managing user inventory
*   managing cross-platform inventory
*   managing virtual currency balance

The `XsollaStoreSubsystem` and` XsollaLoginSubsystem` are wrapper classes for the standard [IHttpRequest](https://docs.unrealengine.com/en-US/API/Runtime/HTTP/Interfaces/IHttpRequest/index.html) class. Since the methods for API calls initiate asynchronous requests to the Xsolla servers, they use [delegates](https://docs.unrealengine.com/en-US/ProgrammingAndScripting/ProgrammingWithCPP/UnrealArchitecture/Delegates/index.html) as parameters. Delegates are called when a response is received from the server.

### Data and classes for working with API

In addition to methods for calling the API, the SDK contains data and classes for making requests to the API and receiving a response.

<div style="background-color: #d9d9d9">
	<p>
		<strong>Example:</strong>
		<ul>
			<li>The <code>FStoreCartItem</code> class describes a cart object (name, type, and other attributes).</li>
			<li>The <code>FXsollaSocialFriendsData</code> is data describing a user's list of friends.</li>
		</ul>
	</p>
</div>

## Demo project module

Blueprints from the _Content_ catalog represent one of the possible logics for the login system, inventory, and in-game store. You can use the demo project in two ways:
*   as an example
*   as an initial version of the login system, inventory, and in-game store in order to get a quick result and expand its capabilities if necessary

Most of the code in the demo project is the ordinary widget blueprints that use the SDK:
*   Page controllers.

<div style="background-color: #d9d9d9">
	<p><strong>Example:</strong></p>
	<p>The <code>W_LoginDemo</code> manages the user's login page. It determines whether the user is authorized, and if not, it can display a sign-up page or a new user registration page.</p>
</div>

*   Blueprints for UI building and managing.

<div style="background-color: #d9d9d9">
	<p><strong>Example:</strong></p>
	<p>The <code>W_CartPanel</code> builds a page with information about the cart data (e.g., the number of items to purchase) and counts and shows the total purchase price, etc.</p>
</div>

*   Pop-up windows displaying various information to the user (e.g., successful purchase information or a field for entering a coupon).