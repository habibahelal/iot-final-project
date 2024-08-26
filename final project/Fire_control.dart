import 'package:flutter/material.dart';

class FireControlPage extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text('Fire Control'),
        backgroundColor: Colors.redAccent,
      ),
      body: Center(
        child: Text(
          'Current Fire Status: No Fire detected',
          style: TextStyle(fontSize: 24.0, fontWeight: FontWeight.bold),
        ),
      ),
    );
  }
}
