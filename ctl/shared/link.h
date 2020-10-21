// TO BE INCLUDED IN OTHER TEMPLATE FILES THAT REQUIRE
// DOUBLE ENDED LINKED LIST NODES TO BE CONNECTED OR DISCONNECTED.
//
// EG. LST.H
//     DEQ.H

static inline void
CTL_IMPL(CTL_A, link_disconnect)(CTL_A* self, CTL_B* node)
{
    if(node == self->tail) self->tail = self->tail->prev;
    if(node == self->head) self->head = self->head->next;
    if(node->prev) node->prev->next = node->next;
    if(node->next) node->next->prev = node->prev;
    node->prev = node->next = NULL;
    self->size -= 1;
}

static inline void
CTL_IMPL(CTL_A, link_connect)(CTL_A* self, CTL_B* position, CTL_B* node, bool before)
{
    if(CTL_IMPL(CTL_A, empty)(self))
        self->head = self->tail = node;
    else
    if(before)
    {
        node->next = position;
        node->prev = position->prev;
        if(position->prev)
            position->prev->next = node;
        position->prev = node;
        if(position == self->head)
            self->head = node;
    }
    else // AFTER.
    {
        node->prev = position;
        node->next = position->next;
        if(position->next)
            position->next->prev = node;
        position->next = node;
        if(position == self->tail)
            self->tail = node;
    }
    self->size += 1;
}
