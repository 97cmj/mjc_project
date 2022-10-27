package com.example.smartwindow;

import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.Toolbar;

import android.content.Intent;
import android.graphics.Color;
import android.os.Bundle;
import android.provider.Settings;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import com.google.firebase.database.DataSnapshot;
import com.google.firebase.database.DatabaseError;
import com.google.firebase.database.DatabaseReference;
import com.google.firebase.database.FirebaseDatabase;
import com.google.firebase.database.ValueEventListener;

public class MainActivity extends AppCompatActivity {

    TextView readText;

    EditText writeEdit;

    Toolbar myToolbar;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        readText = findViewById(R.id.temp_EditText);
        Button readBtn = findViewById(R.id.read_btn);


        myToolbar = (Toolbar) findViewById(R.id.my_toolbar);
        setSupportActionBar(myToolbar);
        getSupportActionBar().setDisplayShowTitleEnabled(false);


        
        
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

    public boolean onCreateOptionsMenu(Menu menu) {
        //return super.onCreateOptionsMenu(menu);
        MenuInflater menuInflater = getMenuInflater();
        menuInflater.inflate(R.menu.menu, menu);
        return true;
    }


    public boolean onOptionsItemSelected(MenuItem item) {
        //return super.onOptionsItemSelected(item);
        switch (item.getItemId()) {
            case R.id.action_settings:

                Intent intent = new Intent(getApplicationContext(), SettingsActivity.class);
                startActivity(intent);

        }
        return true;
    }

}