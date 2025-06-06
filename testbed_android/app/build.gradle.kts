plugins {
    alias(libs.plugins.android.application)
    alias(libs.plugins.kotlin.android)
}

android {
    namespace = "com.mosaic.testbed"
    compileSdk = 35
    ndkVersion = "29.0.13113456"

    defaultConfig {
        applicationId = "com.mosaic.testbed"
        minSdk = 30
        targetSdk = 35
        versionCode = 1
        versionName = "1.0"

        testInstrumentationRunner = "androidx.test.runner.AndroidJUnitRunner"

        ndk {
            abiFilters.add("arm64-v8a") // Match your triplet
        }
    }

    buildTypes {
        release {
            isMinifyEnabled = false
            proguardFiles(
                    getDefaultProguardFile("proguard-android-optimize.txt"),
                    "proguard-rules.pro"
            )
        }
    }

    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_11
        targetCompatibility = JavaVersion.VERSION_11
    }

    kotlinOptions { jvmTarget = "11" }

    buildFeatures { prefab = true }

    externalNativeBuild {
        cmake {
            path = file("../../CMakeLists.txt")
            version = "3.31.0"
        }
    }

    val commonCmakeArgs =
            listOf(
                    "-DANDROID=TRUE",
                    "-DCMAKE_FIND_ROOT_PATH_MODE_PACKAGE=BOTH",
                    "-DCMAKE_TOOLCHAIN_FILE=${System.getenv("VCPKG_ROOT")}/scripts/buildsystems/vcpkg.cmake",
                    "-DVCPKG_CHAINLOAD_TOOLCHAIN_FILE=${System.getenv("ANDROID_NDK_HOME")}/build/cmake/android.toolchain.cmake",
                    "-DVCPKG_TARGET_TRIPLET=arm64-android",
                    "-DANDROID_STL=c++_shared",
                    "-DANDROID_CPP_FEATURES=rtti exceptions",
                    "-DCMAKE_FIND_ROOT_PATH_MODE_PACKAGE=BOTH"
            )

    buildTypes {
        debug {
            externalNativeBuild {
                cmake {
                    arguments.addAll(commonCmakeArgs)
                    arguments.add("-DCMAKE_BUILD_TYPE=Debug")
                }
            }
        }

        create("dev") {
            isDebuggable = true
            isJniDebuggable = false

            externalNativeBuild {
                cmake {
                    arguments.addAll(commonCmakeArgs)
                    arguments.add("-DCMAKE_BUILD_TYPE=Dev")
                }
            }
        }

        release {
            isMinifyEnabled = false

            proguardFiles(
                    getDefaultProguardFile("proguard-android-optimize.txt"),
                    "proguard-rules.pro"
            )

            externalNativeBuild {
                cmake {
                    arguments.addAll(commonCmakeArgs)
                    arguments.add("-DCMAKE_BUILD_TYPE=Release")
                }
            }
        }
    }
}

dependencies {
    implementation(libs.androidx.core.ktx)
    implementation(libs.androidx.appcompat)
    implementation(libs.material)
    implementation(libs.androidx.games.activity)
    testImplementation(libs.junit)
    androidTestImplementation(libs.androidx.junit)
    androidTestImplementation(libs.androidx.espresso.core)
}
