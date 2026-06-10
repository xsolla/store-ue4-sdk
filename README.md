# store-ue4-sdk

[![License](https://img.shields.io/badge/license-Apache%202.0-blue.svg)](./LICENSE.txt)
[![Latest Release](https://img.shields.io/github/v/release/xsolla/store-ue4-sdk)](https://github.com/xsolla/store-ue4-sdk/releases)
[![Unreal Engine 5.3+](https://img.shields.io/badge/Unreal%20Engine-5.3%2B-black.svg?logo=unrealengine)](https://www.unrealengine.com/)

## Overview

The Xsolla SDK for Unreal Engine makes it easier to integrate Xsolla products into your Unreal Engine project by providing out-of-the-box data structures and methods for working with the Xsolla API. After integration, you can use Xsolla Login for user authentication, In-Game Store for managing purchases and player inventory, Pay Station for payments, and Subscriptions for recurring access plans.

## Requirements

- **OS (development):** 64-bit Windows 7+ or macOS 13.6.6+
- **Unreal Engine:** 5.3 or later
- **Target platforms:** iOS, Android, Windows 64-bit
- **Xsolla Publisher Account:** Required — [sign up here](https://publisher.xsolla.com/signup?store_type=sdk)

## Install

1. Clone this repository into your project's `Plugins/` directory:

```bash
git clone https://github.com/xsolla/store-ue4-sdk.git Plugins/Xsolla
```

2. Regenerate project files (right-click your `.uproject` → **Generate Visual Studio project files**).
3. Build and open your project in Unreal Engine.
4. Follow the [integration guide](https://developers.xsolla.com/sdk/unreal-engine/integrate-complete-solution/).

## Usage

```cpp
UXsollaStoreSubsystem* StoreSubsystem = GEngine->GetEngineSubsystem<UXsollaStoreSubsystem>();
StoreSubsystem->UpdateVirtualItems(ProjectId, FOnStoreUpdate::CreateUObject(
    this, &UMyClass::OnItemsUpdated));
```

## Documentation

Full SDK documentation: [developers.xsolla.com/sdk/unreal-engine/](https://developers.xsolla.com/sdk/unreal-engine/)

## Support

- [GitHub Issues](https://github.com/xsolla/store-ue4-sdk/issues)
- [Xsolla partner support](https://xsolla.com/partner-support)
- [Integration team](mailto:integration@xsolla.com)

## License

Apache 2.0. See [LICENSE](./LICENSE.txt).
