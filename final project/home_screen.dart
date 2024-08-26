import 'package:flutter/material.dart';
import 'package:firebase_auth/firebase_auth.dart';
import 'package:flutter_firebase1/Fire_control.dart';
import 'led_control.dart'; // Import the LED control page
import 'Fire_control.dart'; // Import the Fire control page
import 'rain_control.dart'; // Import the Rain control page

User? loggedinUser;

class HomeScreen extends StatefulWidget {
  @override
  _HomeScreenState createState() => _HomeScreenState();
}

class _HomeScreenState extends State<HomeScreen> {
  final _auth = FirebaseAuth.instance;

  @override
  void initState() {
    super.initState();
    getCurrentUser();
  }

  // Function to get the current user
  void getCurrentUser() async {
    try {
      final user = await _auth.currentUser;
      if (user != null) {
        loggedinUser = user;
      }
    } catch (e) {
      print(e);
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        leading: null,
        actions: <Widget>[
          IconButton(
            icon: Icon(Icons.close),
            onPressed: () {
              _auth.signOut();
              Navigator.pop(context);
            },
          ),
        ],
        title: Text('Home Page'),
        backgroundColor: Colors.lightBlueAccent,
      ),
      body: Column(
        mainAxisAlignment: MainAxisAlignment.center,
        crossAxisAlignment: CrossAxisAlignment.center,
        children: <Widget>[
          Text(
            "Welcome User",
            style: TextStyle(fontSize: 24.0, fontWeight: FontWeight.bold),
          ),
          SizedBox(height: 20), // Add some space between text and buttons
          ElevatedButton(
            onPressed: () {
              // Navigate to LED Control page
              Navigator.push(
                context,
                MaterialPageRoute(builder: (context) => LedControlPage()),
              );
            },
            child: Text('LED Control'),
          ),
          SizedBox(height: 10), // Add space between buttons
          ElevatedButton(
            onPressed: () {
              // Navigate to Fire Control page
              Navigator.push(
                context,
                MaterialPageRoute(builder: (context) => FireControlPage()),
              );
            },
            child: Text('Fire Control'),
          ),
          SizedBox(height: 10), // Add space between buttons
          ElevatedButton(
            onPressed: () {
              // Navigate to Rain Control page
              Navigator.push(
                context,
                MaterialPageRoute(builder: (context) => RainControlPage()),
              );
            },
            child: Text('Rain Control'),
          ),
        ],
      ),
    );
  }
}
