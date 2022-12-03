#include"MentOS.h"


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
    printf_color("\n- Author: Gabriel Gao, Tim Zhao, Tony Wang, Jerry Wang\n",0x3);
    printf_color("- Created Time: 15 October - 4th December\n",0x6);
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
    // UI_author();
    // UI_time();
    printf("\n\n Starting the MentOS...\n");
    printf("Starting 391 Shell\n");
    printf("391OS> ");

}

void UI_END(){
    printf("Thank you for using the MentOS");
    return;
}
