$(document).ready(function(){
    $('.submit-button').click(function(e) {
        
        var integrity = true;
        var message = "";
        /* Question - 1 */

        if (answer[0] == 1) {
            $(".question-1").val("a1");
        }

        else if (answer[1] == 1) {
            $(".question-1").val("a2");
        }

        else if (answer[2] == 1) {
            $(".question-1").val("a3");
        }

        else {
            message += "الرجاء التحديد: السؤال - 1\n";
            integrity = false;
        }

        /* Question - 2 */

        if (answer[3] == 1) {
            $(".question-2").val("a1");
        }

        else if (answer[4] == 1) {
            $(".question-2").val("a2");
        }

        else if (answer[5] == 1) {
            $(".question-2").val("a3");
        }

        else {
            message += "الرجاء التحديد: السؤال - 2\n";
            integrity = false;
        }

        /* Question - 3 */

        if (answer[6] == 1) {
            $(".question-3").val("a1");
        }

        else if (answer[7] == 1) {
            $(".question-3").val("a2");
        }

        else if (answer[8] == 1) {
            $(".question-3").val("a3");
        }

        else {
            message += "الرجاء التحديد: السؤال - 3\n";
            integrity = false;
        }

        /* Initiate Submit Action */
        if (integrity) {
            $(".form-submit").click();
        }

        else {
            alert(message);
        }

    });
});