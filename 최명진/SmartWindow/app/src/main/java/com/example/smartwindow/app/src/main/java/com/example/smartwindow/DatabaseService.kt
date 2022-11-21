package com.example.smartwindow

import com.google.firebase.database.DatabaseReference
import com.google.firebase.database.FirebaseDatabase

class DatabaseService {


    //쓰기
    val database = FirebaseDatabase.getInstance()
    val myRef = database.getReference()



}