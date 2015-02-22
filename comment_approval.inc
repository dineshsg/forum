<?php

function comment_waiting_for_approval()
    {
        global $user;
        $c_id = arg(3);
        $c_status = arg(4);
        if(($c_status==1)||($c_status==-1))
        {
            if($c_status==1)
            {
                /*---------------------------------------------
                    Question which are approved by the Moderator
                ------------------------------------------------*/
                $query_app = "UPDATE `forum_question_reply` SET `status`='".$c_status."', `updated` = CURRENT_TIMESTAMP WHERE id = '".$c_id."'";            
                if(db_query($query_app)){
                    drupal_set_message('New Comment has been posted on Forum');
                }else {
                    drupal_set_message('Error !!!!.Please try again.');
                }
                drupal_goto('forum/comment/approval');
            }
            else
            {
                /*----------------------------------------------
                    Question which are rejected by the Moderator
                ------------------------------------------------*/
                $query_rej = "UPDATE `forum_question_reply` SET `status`='".$c_status."', `updated` = CURRENT_TIMESTAMP WHERE id = '".$c_id."'";            
                if(db_query($query_rej)){
                    drupal_set_message('New Comment has been Rejected');
                }else {
                    drupal_set_message('Error !!!!.Please try again.');
                }
                drupal_goto('forum/comment/approval');
            }
            
        }
        else
        {
                /*------------------------------------------------------------
                    List of comment which are waiting for moderators approval
                --------------------------------------------------------------*/
            if ((in_array('moderator', $user->roles)) || (in_array('admin moderator', $user->roles))||($user->uid == 1))
            {
                $output = '';
                $row = array();
                $header = array(
                            array('data' => 'Question', 'field' => 'question', 'sort' => 'asc'),
                            array('data' => 'Comment', 'field' => 'reply_message', 'sort' => 'asc'),
                            array('data' => 'Posted By', 'field' => 'user_id', 'sort' => 'asc'),
                            array('data' =>'Action', 'field' => '', 'sort' => 'asc')
                            );
                $result = db_select('forum_questions','fq');
                $result->join('forum_question_reply','fqr', 'fqr.question_id = fq.id');
                $result->fields('fq', array('question'));
                $result->fields('fqr', array('reply_message','user_id','created','id'));
                if (in_array('moderator', $user->roles))
                {
                    /*------------------------------------
                           Check for Moderator Roles
                    --------------------------------------*/
                    $moderator_query = db_query('SELECT * FROM users_extend WHERE uid = '.$user->uid)->fetchAll();
                    $myArray = explode(',', $moderator_query[0]->subject);
                    $result->condition('fq.subject_id', $myArray, 'IN');
                    $result->condition('fqr.status', 0)
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
                       ->orderBy('created', 'ASC')
                       ->limit(5);
                $result = $pager->execute();
                if(!$result->rowCount())
                {
                    $output .= "No Comment for approval!!";
                }
                else
                {
                    while ($item = $result->fetch())
                    {
                        $user_post = user_load($item->user_id);
                        $username = $user_post->name;
                        $date_ago = ago($item->created);
                        $row[] = array($item->question,$item->reply_message,$username.'<div class="algn-right small-font">'.$date_ago.'</div>',l('Accept','forum/comment/approval/'.$item->id.'/1').' | '.l('Reject','forum/comment/approval/'.$item->id.'/-1'));
                    }
                    $output .= theme('table',array('header' => $header,'rows' => $row));
                    $output .= theme('pager');
                }
        	}
            else
            {
                drupal_set_message('Please ask your Moderator to provide access');
                $output = '';
            }
            return $output;
        }
    }
?>
