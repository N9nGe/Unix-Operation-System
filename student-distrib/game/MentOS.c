#include"MentOS.h"
/*MentOS UI
 * Here is a series of extra function we developped for fun
 * All the stuff in it are not connected to the MP3 grading criterion
 *  
 * Please ignore the comment loss and function header loss in it, because we are still 
 * developping GUI in another branch
 * :-) 
*/

void UI_Aligater(){
printf("        :::   :::   :::::::::: ::::    ::: ::::::::::: ::::::::   ::::::::\n");
printf("      :+:+: :+:+:  :+:        :+:+:   :+:     :+:    :+:    :+: :+:    :+: \n");
printf("    +:+ +:+:+ +:+ +:+        :+:+:+  +:+     +:+    +:+    +:+ +:+         \n");
printf("   +#+  +:+  +#+ +#++:++#   +#+ +:+ +#+     +#+    +#+    +:+ +#++:++#++   \n");
printf("  +#+       +#+ +#+        +#+  +#+#+#     +#+    +#+    +#+        +#+    \n");
printf(" #+#       #+# #+#        #+#   #+#+#     #+#    #+#    #+# #+#    #+#     \n");
printf("###       ### ########## ###    ####     ###     ########   ######## \n");

}

void UI_author(){
    printf("\n- Author: Gabriel Gao, Tim Zhao, Tony Wang, Jerry Wang\n");
    printf("- Created Time: 15 October - 4th December\n");
}


void UI_time(){
    //TODO : is real time 
    int year = 2022;
    int month = 12;
    int day = 4;

    printf("\n             Present time is %d/%d/%d \n",month,day,year);
}

void UI_START(){

    UI_Aligater();
    UI_author();
    // UI_time();
    printf("\n\n Starting the MentOS...\n");
    printf("Welcome to MentOS!\n\n");
    // This two lines are left to fix the bug of first shell being clear()
    printf("Starting 391 Shell\n");
    printf("391OS> "); 

}

void UI_END(){
    printf("Thank you for using the MentOS");
    return;
}
