<?php
    /*-------------------------------------------
        Questions waiting for Moderators Approval
    ----------------------------------------------*/
    function question_waiting_for_approval()
    {
        global $user;
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
                    drupal_set_message('Question has Approved');
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
                    drupal_set_message('Question has been Rejected');
                }else {
                    drupal_set_message('Error !!!!.Please try again.');
                }

                $stu_query = db_query("SELECT user_id,grade_id,subject_id,question FROM forum_questions WHERE id = ".$q_id)->fetchObject();
                $user = user_load($stu_query->user_id);
                
                $message = array('0'=>t('Dear Student,<br><br>A question has been posted on forums and is waiting for the approval<br>
                                 Grade: '.grade($stu_query->grade_id).'<br>
                                 Topic: '.subject($stu_query->subject_id).'<br>
                                 Question: '.$stu_query->question.'')); // Body of your email here.
                
                $params = array(
                    'body' => $message,
                    'subject' => 'Question has been Rejected by the Moderator',
                    'headers'=> array('Content-Type' => 'text/html'),
                    //'attachment' => $attachment,
                );
                $to = $user->mail;
                $language = user_preferred_language($user);
                drupal_mail('forum', 'rejectaquestion', $to, $language, $params, 'demo@demo.com', $send = TRUE);
                drupal_goto('forum/question/approval');
                
            }
            
        }
        else
        {
                /*------------------------------------------------------------
                    List of Questions which are waiting for moderators approval
                --------------------------------------------------------------*/
            if ((in_array('moderator', $user->roles)) || (in_array('admin moderator', $user->roles)) || ($user->uid == 1))
            {
                $output = '';
                $row = array();
                $header = array(
                            array('data' => 'Grade', 'field' => 'subject_name', 'sort' => 'asc'),
                            array('data' => 'Topic', 'field' => 'grade_name', 'sort' => 'asc'),
                            array('data' => 'Question', 'field' => 'question', 'sort' => 'asc'),
							array('data' => 'Attachment', 'field' => 'id', 'sort' => 'asc'),
                            array('data' => 'Posted By', 'field' => 'user_id', 'sort' => 'asc'),
                            array('data' =>'Action', 'field' => '', 'sort' => 'asc')
                            );
                $result = db_select('forum_questions','fq');
                $result->join('forum_grades','fg', 'fq.grade_id = fg.id');
                $result->join('forum_subjects','fs', 'fq.subject_id = fs.id');
                $result->fields('fs', array('subject_name'));
                $result->fields('fg', array('grade_name'));
                $result->fields('fq', array('question','user_id','created','id'));
				$result->condition('fq.status',0,'=');
                if (in_array('moderator', $user->roles))
                {
                    /*------------------------------------
                           Check for Moderator Roles
                    --------------------------------------*/
                    $moderator_query = db_query('SELECT * FROM users_extend WHERE uid = '.$user->uid)->fetchAll();
                    $myArray = explode(',', $moderator_query[0]->subject);
                    $result->condition('fq.subject_id', $myArray, 'IN');
                    $result->condition('status', 0)
                           ->execute()
                           ->fetchAll();
                }
                else if(in_array('admin moderator', $user->roles))
                {
                    /*------------------------------------
                         Check for Admin Moderator Roles
                    --------------------------------------*/
                    $result->condition('status', 0)
                           ->execute()
                           ->fetchAll();
                }
                else
                {
                    /*------------------------------------
                             Check for Other Roles
                    --------------------------------------*/
                }
                $table_sort = $result->extend('TableSort')
                            ->orderByHeader($header);
                $pager = $table_sort->extend('PagerDefault')
                       ->orderBy('created', 'DESC')
                       ->limit(5);
                $result = $pager->execute();
                if(!$result->rowCount())
                {
                    $output .= "No Questions for approval!!";
                }
                else
                {
                    while ($item = $result->fetch())
                    {
						$temp_user_post=user_load($item->user_id);
						$post_user=$temp_user_post->name;
                        $date_ago = ago($item->created);
						
						////-- Added by manjiri --///////
						$attacment_query = db_query('SELECT question_id, file_name FROM forum_question_attach WHERE question_id = '.$item->id);
						$link="-";
						while ($row_attachment = $attacment_query->fetchObject())
						{					
						//////-- End Added by manjiri --/////						
                        /*$row[] = array($item->grade_name,$item->subject_name,$item->question,$post_user.'<div class="algn-right small-font">'.$date_ago.'</div>',l('Accept','forum/question/approval/'.$item->id.'/1').' | '.l('Reject','forum/question/approval/'.$item->id.'/-1'));*/
						if($row_attachment->file_name)
							$link="<a href='../../".$row_attachment->file_name."' target='_blank'>File</a>";
						else
							$link="-";
						}
						$row[] = array($item->grade_name,$item->subject_name,$item->question,$link, $post_user.'<div class="algn-right small-font">'.$date_ago.'</div>',l('Accept','forum/question/approval/'.$item->id.'/1').' | '.l('Reject','forum/question/approval/'.$item->id.'/-1'));
						
						//-- Added by manjiri --///
						//}
						//-- End Added by manjiri --///
                    }
                    $output .= theme('table',array('header' => $header,'rows' => $row));
                    $output .= theme('pager');
                }
    	    
            }
            else
            {
                drupal_set_message('You dont have access-rights to view this page.');
                $output = '';
            }
            return $output;
        }
    }
    
?>
