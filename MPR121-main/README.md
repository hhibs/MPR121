
//	  Test code for MPR121 Touch

	  result = MPR_121_begin( &hi2c1, 0x5A<<1 , MPR121_TOUCH_THRESHOLD_DEFAULT , MPR121_RELEASE_THRESHOLD_DEFAULT );
	  if(result == HAL_OK)
	  {
		  HAL_UART_Transmit(&huart2,str2, 17, 100);
	  }

	  while (1)
	  {
	    /* USER CODE END WHILE */
	    /* USER CODE BEGIN 3 */

		  uint16_t touces = 0;

		  result = MPR_121_touched( &hi2c1, 0x5A<<1 , &touces );
		  if(result == HAL_OK )
		  {
			  for (uint8_t i = 0 ; i < 12 ;  i++)
			  {
				 if((( touces >> i ) & 0x01) == 0x01)
				 {

					 if(i==0)
					 HAL_UART_Transmit( &huart2 , "Button 0\r\n" , 10 , 5000 );

					 if(i==1)
					 HAL_UART_Transmit( &huart2 , "Button 1\r\n" , 10 , 5000 );

					 if(i==2)
					 HAL_UART_Transmit( &huart2 , "Button 2\r\n" , 10 , 5000 );

					 if(i==3)
					 HAL_UART_Transmit( &huart2 , "Button 3\r\n" , 10 , 5000 );

					 if(i==4)
					 HAL_UART_Transmit( &huart2 , "Button 4\r\n" , 10 , 5000 );

					 if(i==5)
					 HAL_UART_Transmit( &huart2 , "Button 5\r\n" , 10 , 5000 );

					 if(i==6)
					 HAL_UART_Transmit( &huart2 , "Button 6\r\n" , 10 , 5000 );

					 if(i==7)
					 HAL_UART_Transmit( &huart2 , "Button 7\r\n" , 10 , 5000 );

					 if(i==8)
					 HAL_UART_Transmit( &huart2 , "Button 8\r\n" , 10 , 5000 );

					 if(i==9)
					 HAL_UART_Transmit( &huart2 , "Button 9\r\n" , 10 , 5000 );

					 if(i==10)
					 HAL_UART_Transmit( &huart2 , "Button 10\r\n" , 11 , 5000 );

					 if(i==11)
					 HAL_UART_Transmit( &huart2 , "Button 11\r\n" , 11 , 5000 );

				 }
			  }
		  }
	  }