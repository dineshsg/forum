<?php

    function question_det_approval()
        {
        global $user;
        global $base_url;
        $account = $user->uid;
        $question_id = arg(2);
        $output = '';
        if($question_id)
        {
            $question_detail = db_query('SELECT grds.grade_name, sub.subject_name, ques.question, ques.user_id, ques.created, ques.id FROM forum_grades as grds, forum_subjects as sub, forum_questions as ques WHERE ques.grade_id = grds.id and ques.subject_id = sub.id and ques.id = '.$question_id.'');		
		    $res_attachment = db_query('SELECT id,question_id,file_name from forum_question_attach where question_id='.$question_id.'');
            $output .= "<div>";
  	        while($rows = $question_detail->fetchObject()){
			    $grade_name=$rows->grade_name;
			    $subject_name=$rows->subject_name;
		        $output .= "<div class='col-12 img_bg border_blue'>
		                        <div class='img_bg '>
		                            <div class='space'>
  	                                    <div class='font-100 custom_font2 content_down '>
  	                                    Grade :<label class='font_black font-300'> ".$rows->grade_name." </label> &nbsp; &nbsp;              
  	                                    Topic :<label class='font_black font-300'> ".$rows->subject_name." </label>
  	                                </div>
  	                            </div>";
  	    
                $date_ago = ago($rows->created);
	            $temp_user = user_load($rows->user_id);
                $temp_username = $temp_user->name;
                $output .= "<div class='space content-font'>
                                <div>
                        	        <span>Posted By:</span><strong><span>".$temp_username."</span></strong>
                                </div>
                                <div>
                        	        <span>Date - </span><strong><span>".date('F jS Y', strtotime($rows->created))."</span></strong>
                                </div>";
		 			
                $output .= "<div class='content-font'>
                                <div>Question:</div>
                                <h5 class='font-300 font-black'><p style='text-indent: auto;'><strong>".nl2br($rows->question)."</strong></p></h5>";
				        		while($rows_attach = $res_attachment->fetchObject()){
		 		        			if($rows_attach->file_name)	
		 		        				$output .="<a href='".base_path().$rows_attach->file_name."' target='_blank' class='font_blue'>See file attached</a><br />";
				            		}
			    $output .="</div></div>";	
            }
        $question_detail1 = db_query('SELECT ques.grade_id, ques.subject_id, ques.user_id FROM forum_questions as ques WHERE ques.id = '.$question_id.'');
        $row = $question_detail1->fetchObject();
        $teacher_query = db_query("SELECT * FROM users_extend WHERE uid = '".$user->uid."' AND grade = '".$row->grade_id."' AND FIND_IN_SET('".$row->subject_id."', subject)");
		
        if((!user_is_logged_in())||(in_array('authenticated user', $user->roles)))
        {
		}
        else
        {
            drupal_set_message('You dont have access-rights to view this page','warning');
        }		
        $grd_subj = db_query("SELECT grade_id,subject_id FROM forum_questions WHERE id=".$question_id)->fetchObject();
		$grades = $grd_subj->grade_id;
		$subjects = $grd_subj->subject_id;
		$student_query = db_query('SELECT count(*) as cnt FROM users_extend  WHERE uid = '.$account.' AND grade = '.$grades.' AND FIND_IN_SET('.$subjects.',subject) > 0')->fetchObject();
        if((in_array('admin moderator',$user->roles))||((in_array('moderator',$user->roles)&&($student_query->cnt > 0))||(in_array('administrator',$user->roles))))
        {
           $output.="<div class='space content_down'><a href='".$base_url."/forum/question/approval/".$question_id."/1' class='btn btn-default'>Accept</a> <a href='".$base_url."/forum/question/approval/".$question_id."/-1' class='btn btn-default reject'>Reject</a><br /><br /></div>";
        }
        $output .= "</div><br><div class='space'><a href='../../question/approval' class='btn btn-default'>Go to question apprval</a></div><br /></div>";
        }
        else
        {
            $output.="No Questions have been posted!"; 
        }
        return $output;
    }

?>
