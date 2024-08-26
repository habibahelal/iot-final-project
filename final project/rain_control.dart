import 'package:flutter/material.dart';

class RainControlPage extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text('Rain Control'),
        backgroundColor: Colors.blueAccent,
      ),
      body: Center(
        child: Text(
          'Current Rain Status: NO Rain detected',
          style: TextStyle(fontSize: 24.0, fontWeight: FontWeight.bold),
        ),
      ),
    );
  }
}
