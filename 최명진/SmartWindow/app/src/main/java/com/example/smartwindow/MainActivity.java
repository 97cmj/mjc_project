package com.example.smartwindow;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

import com.google.firebase.database.DataSnapshot;
import com.google.firebase.database.DatabaseError;
import com.google.firebase.database.DatabaseReference;
import com.google.firebase.database.FirebaseDatabase;
import com.google.firebase.database.ValueEventListener;

public class MainActivity extends AppCompatActivity {

    TextView readText;

    EditText writeEdit;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        readText = findViewById(R.id.read_text);
        Button readBtn = findViewById(R.id.read_btn);
        readBtn.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent = new Intent(getApplicationContext(), CctvActivity.class);
                startActivity(intent);
            }
        });



                //쓰기
        FirebaseDatabase database = FirebaseDatabase.getInstance();
                //DatabaseReference myRef = database.getReference("dht");
        DatabaseReference myRef = database.getReference("dht");
                // 읽기
        myRef.addValueEventListener(new ValueEventListener() {
            @Override
            public void onDataChange(DataSnapshot dataSnapshot) {
                // This method is called once with the initial value and again
                // whenever data at this location is updated.
                String heat = dataSnapshot.child("heat").getValue(String.class);
                String temp = dataSnapshot.child("temp").getValue(String.class);
                String humi = dataSnapshot.child("humi").getValue(String.class);
                String gas = dataSnapshot.child("gas").getValue(String.class);
                String rain = dataSnapshot.child("rain").getValue(String.class);
                readText.setText("습도: " + humi + "%" + "\n" + "온도: " +  temp + "°C" + "\n" + "체감온도: " + heat +"°C" +"\n" +
                                "가스수치: " + gas + "\n" + "빗물지수 : " + rain);
            }
            @Override
            public void onCancelled(DatabaseError error) {
                // Failed to read value
                readText.setText("error: " + error.toException());
                    }
            });
    }
}