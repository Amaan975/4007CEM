#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <string>
#include <iostream>
#include <sqlite3.h>
#include <vector>
using namespace std;

struct table_data //this struct is for retrieving the data from a query and save it for use in main funtion
{
   int argc;
   char **argv;
   char **azColName;
};

struct timetable_entry //this is the booking entry that we want to store
{
   char *mId;
   char *teacherName;
   char *cId;
   int start_time;
   int end_time;
};

/*As the sqlite_exec() function called for each record so we 
  are storing the records in a vector for future use.*/
using Records = std::vector<table_data *>; 


/*Callback function will populate the global vector named as Records
  it stores all the records in a single entry named as table_data
  it store the three things 
  - total number of arguments (argc)
  - a double pointer char array for the data
  - a double pointer char array for the cloumn names*/
static int callback(void *data, int argc, char **argv, char **azColName)
{

   Records *records = static_cast<Records *>(data);

   table_data *table_data = (struct table_data *)malloc(sizeof(struct table_data));

   table_data->argc = argc;
   table_data->argv = new char *[argc];
   for (int i = 0; i < argc; i++) //allocate memory to the array in the table_data structure
   {
      table_data->argv[i] = new char[60];
   }

   for (int i = 0; i < argc; i++)
   {
      strcpy(table_data->argv[i], argv[i]);
   }

   table_data->azColName = new char *[argc];
   for (int i = 0; i < argc; i++)
   {
      table_data->azColName[i] = new char[60];
   }

   for (int i = 0; i < argc; i++)
   {
      strcpy(table_data->azColName[i], azColName[i]);
   }
   records->push_back(table_data);
   return 0;
}

/*This function will convert the small letters in a string to the capital letters*/
void toUpper(char mName[])
{
   for (int i = 0; mName[i] != '\0'; i++)
   {
      if (mName[i] <= 'z' && mName[i] >= 'a')
      {
         int val = mName[i];
         val -= 32;
         mName[i] = (char)val;
      }
   }
}

//this function will print the final time table for a student
void print_timetable(sqlite3* db)
{
   cout << "*********************Here is the final time table*********************"<<endl; // display final time table
   char *zErrMsg = 0;
   int rc;
   Records records;
   char query1[200] = "SELECT m.moduleName as Module, B.teacherName as Teacher, B.cId as ClassRoom, B.startTime as Start, B.endTime as End from Bookings as B LEFT JOIN Modules as M ON B.mId = M.mId";
   /* Execute SQL statement */
   rc = sqlite3_exec(db, query1, callback, &records, &zErrMsg);

   if (rc != SQLITE_OK)
   {
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }

   int i;
   for(table_data* tb : records)
   {
      for(i = 0; i<tb->argc; i++) {
         cout << tb->azColName[i] << " : " <<tb->argv[i] << endl;
      }
      cout << "--------------------------------------------------------"<<endl;
      cout << endl;  
   }
   cout << endl;
}


int main()
{
   //Create and connect database named as test.db
   sqlite3 *db;
   char *zErrMsg = 0;
   int rc;
   const char *query;

   rc = sqlite3_open("timeTable.db", &db);

   if (rc)
   {
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      return (0);
   }
   else
   {
      fprintf(stderr, "Opened database successfully\n");
   }
// create query 
   query = "create table Modules ("
           "mId char(7),"
           "moduleName varchar(60) NOT NULL,"
           "CONSTRAINT Modules_pk PRIMARY KEY (mId)"
           ")";

   /* Execute SQL statement */
   rc = sqlite3_exec(db, query, callback, 0, &zErrMsg);

   if (rc != SQLITE_OK)
   {
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }
   else
   {
      fprintf(stdout, "Modules Table created successfully\n");
   }
 
//Create the table of Teachers
   query = "create table Teachers("
           "mId char(7) NOT NULL,"
           "teacherName VARCHAR(25) NOT NULL,"
           "CONSTRAINT fk_Modules"
           "   FOREIGN KEY (mId)"
           "   REFERENCES Modules(mId)"
           "CONSTRAINT Teachers_pk PRIMARY KEY (mId,teacherName)"
           ")";

   /* Execute SQL statement */
   rc = sqlite3_exec(db, query, callback, 0, &zErrMsg);

   if (rc != SQLITE_OK)
   {
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }
   else
   {
      fprintf(stdout, "Teachers Table created successfully\n");
   }

   query = "create table Classrooms("
           "cId char(6) primary key"
           ")";

   /* Execute SQL statement */
   rc = sqlite3_exec(db, query, callback, 0, &zErrMsg);

   if (rc != SQLITE_OK)
   {
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }
   else
   {
      fprintf(stdout, "Classrooms Table created successfully\n");
   }
// create table for bookings 
   query = "create table Bookings("
           "mId char(7),"
           "teacherName nvarchar(25),"
           "cId char(6),"
           "startTime int not null,"
           "endTime int not null,"
           "CONSTRAINT fk_Modules"
           "   FOREIGN KEY (mId)"
           "   REFERENCES Modules(mId)"

           "CONSTRAINT fk_Classrooms"
           "   FOREIGN KEY (cId)"
           "   REFERENCES Classrooms(cId)"
           "CONSTRAINT Bookings_pk PRIMARY KEY (mId,teacherName,cId,startTime,endTime)"
           ")";

   /* Execute SQL statement */
   rc = sqlite3_exec(db, query, callback, 0, &zErrMsg);

   if (rc != SQLITE_OK)
   {
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }
   else
   {
      fprintf(stdout, "Classrooms Table created successfully\n");
   }

   /* populating module table with data */
   query = "INSERT INTO Modules (mId,moduleName) "
           "VALUES ('4000CEM','COMPUTER PROGRAMMING and ALGORITHMS');"
           "INSERT INTO Modules (mId,moduleName) "
           "VALUES ('4001CEM','SOFTWARE DESIGN');"
           "INSERT INTO Modules (mId,moduleName) "
           "VALUES ('4002CEM','MATHEMATICS FOR COMPUTER SCIENCE');"
           "INSERT INTO Modules (mId,moduleName) "
           "VALUES ('4003CEM','OBJECT ORIENTED');"
           "INSERT INTO Modules (mId,moduleName) "
           "VALUES ('4004CEM','COMPUTER ARCHITECTURE AND NETWORKS');"
           "INSERT INTO Modules (mId,moduleName) "
           "VALUES ('4005CEM','DATABASE SYSTEMS');"
           "INSERT INTO Modules (mId,moduleName) "
           "VALUES ('4006CEM','PROJECT 1');"
           "INSERT INTO Modules (mId,moduleName) "
           "VALUES ('4007CEM','PROJECT 2');";

   /* Execute SQL statement */
   rc = sqlite3_exec(db, query, callback, 0, &zErrMsg);

   if (rc != SQLITE_OK)
   {
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }
   else
   {
      fprintf(stdout, "Successfully inserted records in Modules\n");
   }

   /* Populating the Classroom table with data */
   query = "INSERT INTO Classrooms (cId) "
           "VALUES ('ECG1');"
           "INSERT INTO Classrooms (cId) "
           "VALUES ('ECG2');"
           "INSERT INTO Classrooms (cId) "
           "VALUES ('ECG3');"
           "INSERT INTO Classrooms (cId) "
           "VALUES ('ECG4');"
           "INSERT INTO Classrooms (cId) "
           "VALUES ('ECG5');"
           "INSERT INTO Classrooms (cId) "
           "VALUES ('ECG6');"
           "INSERT INTO Classrooms (cId) "
           "VALUES ('ECG7');"
           "INSERT INTO Classrooms (cId) "
           "VALUES ('ECG8');"
           "INSERT INTO Classrooms (cId) "
           "VALUES ('ECG9');"
           "INSERT INTO Classrooms (cId) "
           "VALUES ('EC11');"
           "INSERT INTO Classrooms (cId) "
           "VALUES ('EC12');"
           "INSERT INTO Classrooms (cId) "
           "VALUES ('EC13');"
           "INSERT INTO Classrooms (cId) "
           "VALUES ('EC14');"
           "INSERT INTO Classrooms (cId) "
           "VALUES ('EC15');"
           "INSERT INTO Classrooms (cId) "
           "VALUES ('EC16');"
           "INSERT INTO Classrooms (cId) "
           "VALUES ('EC17');"
           "INSERT INTO Classrooms (cId) "
           "VALUES ('EC18');"
           "INSERT INTO Classrooms (cId) "
           "VALUES ('EC19');"
           "INSERT INTO Classrooms (cId) "
           "VALUES ('EC20');"
           "INSERT INTO Classrooms (cId) "
           "VALUES ('EC21');"
           "INSERT INTO Classrooms (cId) "
           "VALUES ('EC22');"
           "INSERT INTO Classrooms (cId) "
           "VALUES ('EC23');"
           "INSERT INTO Classrooms (cId) "
           "VALUES ('EC24');"
           "INSERT INTO Classrooms (cId) "
           "VALUES ('EC25');"
           "INSERT INTO Classrooms (cId) "
           "VALUES ('EC26');"
           "INSERT INTO Classrooms (cId) "
           "VALUES ('EC27');"
           "INSERT INTO Classrooms (cId) "
           "VALUES ('EC28');"
           "INSERT INTO Classrooms (cId) "
           "VALUES ('EC29');";

   /* Execute SQL statement */
   rc = sqlite3_exec(db, query, callback, 0, &zErrMsg);

   if (rc != SQLITE_OK)
   {
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }
   else
   {
      fprintf(stdout, "Successfully inserted records in Classrooms\n");
   }
   //Inserting data in Teachers
   query = "INSERT INTO Teachers (mId,teacherName) "
           "VALUES ('4000CEM','ANDY'); "
           "INSERT INTO Teachers (mId,teacherName) "
           "VALUES ('4000CEM','CHARLES'); "
           "INSERT INTO Teachers (mId,teacherName) "
           "VALUES ('4000CEM','DAVID'); "
           "INSERT INTO Teachers (mId,teacherName) "
           "VALUES ('4001CEM','ALEX'); "
           "INSERT INTO Teachers (mId,teacherName) "
           "VALUES ('4001CEM','CARL'); "
           "INSERT INTO Teachers (mId,teacherName) "
           "VALUES ('4001CEM','JOHN'); "
           "INSERT INTO Teachers (mId,teacherName) "
           "VALUES ('4002CEM','MARY'); "
           "INSERT INTO Teachers (mId,teacherName) "
           "VALUES ('4002CEM','SCOTT'); "
           "INSERT INTO Teachers (mId,teacherName) "
           "VALUES ('4002CEM','ALAN'); "
           "INSERT INTO Teachers (mId,teacherName) "
           "VALUES ('4003CEM','GARY'); "
           "INSERT INTO Teachers (mId,teacherName) "
           "VALUES ('4003CEM','JAMES'); "
           "INSERT INTO Teachers (mId,teacherName) "
           "VALUES ('4003CEM','THOMAS'); "
           "INSERT INTO Teachers (mId,teacherName) "
           "VALUES ('4004CEM','MARK'); "
           "INSERT INTO Teachers (mId,teacherName) "
           "VALUES ('4004CEM','SOPHIE'); "
           "INSERT INTO Teachers (mId,teacherName) "
           "VALUES ('4004CEM','YUSUF'); "
           "INSERT INTO Teachers (mId,teacherName) "
           "VALUES ('4005CEM','UMAR'); "
           "INSERT INTO Teachers (mId,teacherName) "
           "VALUES ('4005CEM','KATIE'); "
           "INSERT INTO Teachers (mId,teacherName) "
           "VALUES ('4005CEM','DIANA'); "
           "INSERT INTO Teachers (mId,teacherName) "
           "VALUES ('4006CEM','KATIE'); "
           "INSERT INTO Teachers (mId,teacherName) "
           "VALUES ('4006CEM','LUCY'); "
           "INSERT INTO Teachers (mId,teacherName) "
           "VALUES ('4006CEM','MARSHALL'); "
           "INSERT INTO Teachers (mId,teacherName) "
           "VALUES ('4007CEM','SIMON'); "
           "INSERT INTO Teachers (mId,teacherName) "
           "VALUES ('4007CEM','JOSEPH'); "
           "INSERT INTO Teachers (mId,teacherName) "
           "VALUES ('4007CEM','BETHANY'); ";

   rc = sqlite3_exec(db, query, callback, 0, &zErrMsg);

   if (rc != SQLITE_OK)
   {
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }
   else
   {
      fprintf(stdout, "Successfully inserted records in Teachers\n");
   }

   ////////////////////////////////////////////////////////////////////////////////////////////
   char mName[60];
   Records records;
   timetable_entry tb;

      //memory allocation to the timetable_entry object

   /*timetable_entry is the final entry which has the data 
      that will finally enter in the booking table. it has
      the following items;
      - Module
      - Teacher Name
      - start_time of class
      - End_time of class
      - Class Room */
   tb.mId = new char[7];
   tb.cId = new char[6];
   tb.teacherName = new char[25];
   tb.start_time = 0;
   tb.end_time = 0;

   const char *data = "Callback function called";

   cout << "********************Welcome to the Coventry University Automated Time Table System*********************" << endl; // display when user turns on system (system header)
   bool next = true;
   while (next)
   {
      cout << "Please Enter the Module Name:   "; // ask user to enter module name 
      cin.getline(mName, 59, '\n'); //User will enter the module name
      toUpper(mName);
      bool check_module_name = true;
      bool check_time_slot = true;
      bool check_teacherName = true;
      bool check_classrooms = true;
      ////////////////////////////////////////////////////////////////////////
      /* Create SQL statement */
      while (check_module_name)
      {
       /* This query will search the entered module in the Modules table */
         char query1[200] = "SELECT * from Modules" // search and extract module information from database 
                            " WHERE moduleName = '";
         strcat(query1, mName);
         strcat(query1, "';");

         /* Execute SQL statement */
         rc = sqlite3_exec(db, query1, callback, &records, &zErrMsg);

         if (rc != SQLITE_OK)
         {
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
         }

         if (records.size() > 0) //this condition check if the entered module exist in table or not
         {
            check_module_name = false;
            strcpy(tb.mId, records[0]->argv[0]); //getting the Module ID from the global container of Query Results
            //now user will enter the time slot
            int time_slot;
            cout << "What time slot would you like (1 hour)?  ";
            cin >> time_slot;
            check_time_slot = true;
            ///////////////////////////////////////////////////Query2////////////////////////////////////
            while (check_time_slot)
            {
              /*This query will search and show a list of teacher that are available
                 that are available for that time that user entered for the classs
                 
                 Note: this query will completely finish the chance of duplicate because user will enter 
                        only those teacher which are available for that time slot*/

               /* SELECT teacherName FROM Teachers WHERE mId = 'Entered_module_name'
                  EXCEPT 
                  SELECT B.teacherName FROM Bookings AS B WHERE B.mId = 'Entered_module_name' AND B.startTime = 'Entered_start_time'*/ 
               char query1[200] = "SELECT teacherName FROM Teachers WHERE mId = '";
               strcat(query1, tb.mId);
               strcat(query1, "' ");
               strcat(query1, "EXCEPT SELECT B.teacherName FROM Bookings AS B WHERE B.mId = '");
               strcat(query1, tb.mId);
               strcat(query1, "' and B.startTime = ");
               string temp_str = to_string(time_slot);
               char *char_type = new char[temp_str.length()];
               strcpy(char_type, temp_str.c_str());
               strcat(query1, char_type);
               strcat(query1, ";");

               //Clearing the container
               records.clear();

               /* Execute SQL statement */
               rc = sqlite3_exec(db, query1, callback, &records, &zErrMsg);

               if (rc != SQLITE_OK)
               {
                  fprintf(stderr, "SQL error: %s\n", zErrMsg);
                  sqlite3_free(zErrMsg);
               }

               if (records.size() <= 0)
               {
                  cout << "No teacher is available in this slot" << endl; // display when no teacher available for time slot
                  cout << "Please choose another slot: " << endl; // ask user to choose another time slot 
                  cin >> time_slot;
               }
               else
               {
                  // check_time_slot = false;
                  tb.start_time = time_slot;
                  tb.end_time = time_slot + 1;

                  cout << "The following teachers are available for this timeslot: " << endl; // show available time slot 
                  for (int i = 0; i < records.size(); i++)
                  {
                     cout << i + 1 << ": " << records[i]->argv[0] << endl;
                  }
                  cout << "Please enter a teacher name:  "; // ask user to enter preffered teacher name from list 
                  check_teacherName = true;
                  getchar();
                  while (check_teacherName)
                  {
                     cin.getline(tb.teacherName, 25);
                     toUpper(tb.teacherName);
                     for (int i = 0; i < records.size(); i++)
                     {
                        if (strcmp(tb.teacherName, records[i]->argv[0]) == 0)
                        {
                           check_teacherName = false;
                           break;
                        }
                     }
                     if (check_teacherName)
                     {
                        cout << "Invalid teacher name! Please try again:  "; // ask the user to enter teacher name again if invalid
                     }
                  }
                  /*This query will search and show a list of classrooms that are available
                  that are available for that time slot that user entered for the module
                  
                  Note: this query will completely finish the chance of duplicate because user will enter 
                           only those teacher which are available for that time slot*/

                  /* SELECT teacherName FROM Classrooms
                     EXCEPT 
                     SELECT B.cId FROM Bookings AS B WHERE B.mId = 'Entered_module_name' AND B.startTime = 'Entered_start_time'*/   
                  char query1[200] = "SELECT cId FROM Classrooms ";
                  strcat(query1, "EXCEPT SELECT B.cId FROM Bookings AS B WHERE ");
                  strcat(query1, "B.startTime = ");
                  string temp_str = to_string(time_slot);
                  char *char_type = new char[temp_str.length()];
                  strcpy(char_type, temp_str.c_str());
                  strcat(query1, char_type);
                  strcat(query1, ";");

                  //Clearing the container
                  records.clear();

                  /* Execute SQL statement */
                  rc = sqlite3_exec(db, query1, callback, &records, &zErrMsg);

                  if (rc != SQLITE_OK)
                  {
                     fprintf(stderr, "SQL error: %s\n", zErrMsg);
                     sqlite3_free(zErrMsg);
                  }

                  if (records.size() <= 0)
                  {
                     cout << "No Classrooms are available in this slot" << endl; // display when no classrooms are available in time slot
                     cout << "Please choose another slot: " << endl; // ask to choose another time slot
                     cin >> time_slot;
                  }
                  else
                  {
                     check_time_slot = false;
                     tb.start_time = time_slot;
                     tb.end_time = time_slot + 1;

                     cout << "The following classrooms are available for this timeslot: " << endl; // displays available classrooms 
                     for (int i = 0; i < records.size(); i++)
                     {
                        cout << i + 1 << ": " << records[i]->argv[0] << endl;
                     }
                     cout << "Please enter a classroom name:  "; // will ask for classroom name
                     check_classrooms = true;
                     while (check_classrooms)
                     {
                        cin.getline(tb.cId, 25);
                        toUpper(tb.cId);
                        cout << "Entered classroom: " << tb.cId << endl; // display the classroom that is entered 
                        for (int i = 0; i < records.size(); i++)
                        {
                           if (strcmp(tb.cId, records[i]->argv[0]) == 0)
                           {
                              check_classrooms = false;

                              break;
                           }
                        }
                        if (check_classrooms)
                        {
                           cout << "Invalid classroom name! Please try again:  "; // if classroom is invalid (not on list) system will ask again 
                        }
                        else
                        {
                           cout << "Classroom found!" << endl;
                           //After all the validation process Inserting booking in Bookings table
                           char query5[250] = "INSERT INTO Bookings (mId,teacherName,cId,startTime,endTime) "
                                              " VALUES ('";
                           strcat(query5, tb.mId);
                           strcat(query5, "','");
                           strcat(query5, tb.teacherName);
                           strcat(query5, "','");
                           strcat(query5, tb.cId);
                           strcat(query5, "','");
                           strcat(query5, char_type);
                           strcat(query5, "','");
                           time_slot++;
                           temp_str = to_string(time_slot);

                           strcpy(char_type, temp_str.c_str());
                           strcat(query5, char_type);
                           strcat(query5, "' );");

                           rc = sqlite3_exec(db, query5, callback, 0, &zErrMsg);

                           if (rc != SQLITE_OK)
                           {
                              fprintf(stderr, "SQL error: %s\n", zErrMsg);
                              sqlite3_free(zErrMsg);
                           }
                           else
                           {
                              records.clear(); // all booking details will be displayed 
                              cout << "Booking Successfull !" << endl << endl;
                              cout << "Booking details are as follows: " << endl;
                              cout << "Module No:   " << tb.mId << endl;
                              cout << "Teacher Name:  " << tb.teacherName << endl;
                              cout << "Classroom No:   " << tb.cId << endl;
                              cout << "Time Slot:  " << tb.start_time << " : 00 - " << tb.end_time << " : 00 " << endl;
                           }
                        }
                     }
                  }
               }
            }
         }
         else
         {
            cout << "Module not found!" << endl; // printed if user entered module that is not on the list
            cout << "Please enter module name again: "; // will ask the user again for the module name 
            cin.getline(mName, 59, '\n');
            toUpper(mName);
         }
      }
      char abc;
      
      cout << "Do you want to enter another booking! (y/n): "<<endl; // system will ask if another booking is required, if so it will carry on if not screen will clear and show all bookings
      cin >> abc;
      system("clear");
      if(abc == 'N' || abc == 'n')
      {
         print_timetable(db);
         break;
      }
   }
   sqlite3_close(db);
   // delete []query;

   return 0;

   //sqlite3_close(db);
}
