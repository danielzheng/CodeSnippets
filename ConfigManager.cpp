void ConfigManager::configUpdate()
{
    const char * configChecksumFile = "/dev/shm/configChecksum";
    int fd = inotify_init();
    if (fd < 0)
    {
        std::cerr << "Could not initialize inotify for configData" << std::endl;
        return;
    }
    int watch_fd = inotify_add_watch(fd, configChecksumFile, IN_CREATE | IN_MODIFY);
    if (watch_fd == -1)
    {
        std::cerr << "Add " << configChecksumFile << " to watch event failed" << std::endl;
        return;
    }
    //monitor file modify and reload configData to buf
    const int buf_len = 8192;
    std::shared_ptr<char> event_buffer(new char[buf_len], std::default_delete<char[]>());

    while (!stop)
    {
        ssize_t i = 0;
        ssize_t length = read(fd, event_buffer.get(), buf_len);
        if (length == 0)
        {
            std::cerr << "read inotify event error: returned 0." << std::endl;
            stop = true;
        }
        else if (length < 0)
        {
            if (errno == EINTR)
            {
                std::cerr << "read inotify interrupted by signal, continue" << std::endl;
                continue;
            }
            else
            {
                std::cerr << "read inotify event error: " << strerror(errno) << std::endl;
                stop = true;
            }
        }
        else
        {
            while (i < length)
            {
                struct inotify_event *event = (struct inotify_event *)&event_buffer.get()[i];
                i += sizeof(struct inotify_event) + event->len; //event->len will always be 0 since not watching dir
                //do readConfigFile
            }
        }
    }
    inotify_rm_watch(fd, watch_fd);
    close(fd);
}
