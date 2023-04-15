package com.vulncom.app;

import org.apache.log4j.Logger;
import org.apache.log4j.LogManager;
import org.apache.log4j.BasicConfigurator;

/**
 * Hello world!
 *
 */
public class App 
{
    // create a static logger
    private static final Logger logger = LogManager.getLogger(App.class);

    public static void main( String[] args )
    {
        // Set up a simple configuration that logs on the console.
        BasicConfigurator.configure();

        System.out.println( "Hello World!" );

        // logging
        logger.debug("Some debug information");
    }
}
