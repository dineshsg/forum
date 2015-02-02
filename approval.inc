<?php
    /*-------------------------------------------
        Questions waiting for Moderators Approval
    ----------------------------------------------*/
    function question_waiting_for_approval()
    {
        $q_id = arg(3);
        $q_status = arg(4);
        if(($q_status==1)||($q_status==-1))
        {
            if($q_status==1)
            {
                /*---------------------------------------------
                    Question which are approved by the Moderator
                ------------------------------------------------*/
                $query_app = "UPDATE `forum_questions` SET `status`='".$q_status."', `updated` = CURRENT_TIMESTAMP WHERE id = '".$q_id."'";            
                if(db_query($query_app)){
                    drupal_set_message('New Question has been posted on Forum');
                }else {
                    drupal_set_message('Error !!!!.Please try again.');
                }
                drupal_goto('forum/question/approval');
            }
            else
            {
                /*----------------------------------------------
                    Question which are rejected by the Moderator
                ------------------------------------------------*/
                $query_rej = "UPDATE `forum_questions` SET `status`='".$q_status."', `updated` = CURRENT_TIMESTAMP WHERE id = '".$q_id."'";            
                if(db_query($query_rej)){
                    drupal_set_message('New Question has been Rejected');
                }else {
                    drupal_set_message('Error !!!!.Please try again.');
                }
                drupal_goto('forum/question/approval');
            }
            
        }
        else
        {
                /*------------------------------------------------------------
                    List of questions which are waiting for moderators approval
                --------------------------------------------------------------*/
        $output = '';
        $question_query = db_query('SELECT grds.grade_name, sub.subject_name, ques.question, ques.user_id, ques.created, ques.id FROM forum_grades as grds, forum_subjects as sub, forum_questions as ques WHERE ques.status = 0 and ques.grade_id = grds.id and ques.subject_id = sub.id');
        if(!$question_query->rowCount())
        {
            $output .= "No Questions for approval!!";
        }
        else
        {
            $output .= "<table class='table table-striped'>";
            $output .= "<th><td>Subject Name</td><td>Grade Name</td><td>Question</td><td>Posted By</td><td>Action</td></th>";
      	    while($rows = $question_query->fetchObject()){
	           $output .= "<tr><td class='img'><span class='question-img'></span></td>";
	           $date_ago = ago($rows->created);
	           $user=user_load($rows->user_id);
               $username=$user->name;
               $output .= "<td>".$rows->grade_name."</td>";
               $output .= "<td>".$rows->subject_name."</td>";
	           $output .= "<td><span><a href='#'>".$rows->question."</a></span><div class='algn-right small-font'>".$date_ago."</div></td>";
	           $output .= "<td class='user-name'>by <span>".$username."</span></td>";
	           $output .= "<td><a href='approval/".$rows->id."/1'>Accept</a> | <a href='approval/".$rows->id."/-1'>Reject</a></td></tr>";
            }
            $output .= '</table>';
        }
	    return $output;
	    }
    }
    
    function ago($timestamp){
        $difference = time() - strtotime($timestamp);
        $text = format_interval($difference, 1) . " ago";
        return $text;    
    }
?>
