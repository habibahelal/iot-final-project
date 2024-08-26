import 'package:flutter/material.dart';
import 'package:firebase_core/firebase_core.dart';
import 'welcome_screen.dart';
import 'home_screen.dart';
import 'signup_screen.dart';
import 'login_screen.dart';
import 'led_control.dart';
import 'Fire_control.dart'; // Import the Fire control page
import 'rain_control.dart';
void main() async{
  WidgetsFlutterBinding.ensureInitialized();
 
 await Firebase.initializeApp(options: const FirebaseOptions(apiKey: "AIzaSyAm5hQSvgyMkJv1GCT14Q_AzJr2XCgG7jU",
  authDomain: "flutterfire-d175b.firebaseapp.com",
  projectId: "flutterfire-d175b",
  storageBucket: "flutterfire-d175b.appspot.com",
  messagingSenderId: "59638620515",
  appId: "1:59638620515:web:0b8230b600420f40717356"));
  
    await Firebase.initializeApp();
  
 

  runApp( MyApp());
}
class MyApp extends StatelessWidget {
  // This widget is the root of your application.
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      initialRoute: 'welcome_screen',
      routes: {
        'welcome_screen': (context) => WelcomeScreen(),
        'registration_screen': (context) => RegistrationScreen(),
        'login_screen': (context) => LoginScreen(),
        'home_screen': (context) => HomeScreen()
        
      },
    );
  }
}
